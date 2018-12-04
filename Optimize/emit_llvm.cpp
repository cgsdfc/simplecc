#include "emit_llvm.h"
#include "SymbolTable.h"
#include "Visitor.h"
#include "error.h"

#include <llvm/ADT/APSInt.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/ErrorHandling.h>

using namespace simplecompiler;

namespace {
using llvm::Value;
using llvm::LLVMContext;
using llvm::Module;
using llvm::IRBuilder;
using llvm::Type;
using llvm::Function;
using llvm::BasicBlock;
using llvm::FunctionType;
using llvm::ConstantInt;
using llvm::ConstantDataArray; /// For string literal
using llvm::APInt;
using llvm::AllocaInst;
using llvm::Constant;

/// A class that map value or ID of a string literal to its llvm representation.
class LLVMStringLiteralTable {
  std::vector<Constant*> IDToValue;
  const StringLiteralTable &StringToID;

  /// StringLiteralTable[String, int] => IDToValue[int, Constant*]
  void InsertLLVMString(LLVMContext &Context) {
    for (const std::pair<String, int> &item : StringToID) {
      auto Val = ConstantDataArray::getString(Context, item.first);
      assert(0 <= item.second && item.second < getStringCount());
      IDToValue[item.second] = Val;
    }
  }

public:
  LLVMStringLiteralTable(const StringLiteralTable &ST, LLVMContext &Context)
    : IDToValue(ST.size()), StringToID(ST) {
    InsertLLVMString(Context);
  }

  /// No copy
  LLVMStringLiteralTable(const LLVMStringLiteralTable &) = delete;
  /// No move
  LLVMStringLiteralTable(LLVMStringLiteralTable &&) = delete;

  unsigned getStringCount() const {
    return StringToID.size();
  }

  /// Return the llvm representation from the ID of a string literal.
  Constant *getFromID(int ID) const {
    assert(0 <= ID && ID < getStringCount() && "StringLiteral ID out of range");
    return IDToValue[ID];
  }

  /// Return the llvm representation from the value of a string literal.
  Constant *getFromValue(const String &Str) const {
    assert(StringToID.count(Str) && "Undefined StringLiteral");
    return getFromID(StringToID.find(Str)->second);
  }
};

/// A class that translates simplecompiler's type system to LLVM's type system.
class LLVMTypeMap {
  LLVMContext &TheContext;
public:
  LLVMTypeMap(LLVMContext &Context): TheContext(Context) {}

  /// Helpers to convert objects of different types to their LLVM counterparts.
  Type *getType(BasicTypeKind BKT) const {
    switch (BKT) {
    case BasicTypeKind::Character:
      return Type::getInt8Ty(TheContext);
    case BasicTypeKind::Int:
      return Type::getInt32Ty(TheContext);
    case BasicTypeKind::Void:
      return Type::getVoidTy(TheContext);
    }
  }

  Type *getType(const ArrayType &A) const {
    return llvm::ArrayType::get(getType(A.GetElementType()), A.GetSize());
  }

  Type *getType(const VarType &V) const {
    return getType(V.GetType());
  }

  Type *getType(const FuncType &F) const {
    Type *ReturnType = getType(F.GetReturnType());
    std::vector<Type *> ArgTypes(F.GetArgCount());
    for (int i = 0; i < F.GetArgCount(); i++) {
      ArgTypes[i] = getType(F.GetArgTypeAt(i));
    }
    return FunctionType::get(ReturnType, ArgTypes, false);
  }
};

class LLVMValueMap : private LLVMTypeMap {
  llvm::Module &TheModule;

  Value *getValueGlobal(const String &Name, const ArrayType &A) const {
    return TheModule.getOrInsertGlobal(Name, getType(A));
  }

  Value *getValueLocal(const String &Name, const ArrayType &A) const {
    return new llvm::AllocaInst(getType(A), /* addressSpace */ 0, Name);
  }

  Value *getValueLocal(const String &Name, const VarType &V) const {
    return new llvm::AllocaInst(getType(V), /* addressSpace */ 0, Name);
  }

  Value *getValueGlobal(const String &Name, const VarType &V) const {
    return TheModule.getOrInsertGlobal(Name, getType(V));
  }

public:
  LLVMValueMap(llvm::Module &M, LLVMContext &Context): LLVMTypeMap(Context), TheModule(M) {}

  Value *getValue(const ConstType &C) const {
    return ConstantInt::get(getType(C.GetType()), C.GetValue(), true);
  }

  Value *getValue(Scope S, const String &Name, const VarType &V) const {
    switch (S) {
    case Scope::Global: return getValueGlobal(Name, V);
    case Scope::Local: return getValueLocal(Name, V);
    }
  }

  Value *getValue(Scope S, const String &Name, const ArrayType &V) const {
    switch (S) {
    case Scope::Global: return getValueGlobal(Name, V);
    case Scope::Local: return getValueLocal(Name, V);
    }
  }

  Value *getValue(const String &Name, const FuncType &F) const {
    Type *ReturnType = getType(F.GetReturnType());
    std::vector<Type *> ArgTypes(F.GetArgCount());
    for (int i = 0; i < F.GetArgCount(); i++) {
      ArgTypes[i] = getType(F.GetArgTypeAt(i));
    }
    FunctionType *FT = FunctionType::get(ReturnType, ArgTypes, false);
    return TheModule.getOrInsertGlobal(Name, FT);
  }

};

/// A class that translates one SymbolTableView of names to their LLVM Values.
class LLVMLocalValueTable {
  std::unordered_map<String, Value*> NamedValues;

  Value *ValueFromSymbolEntry(const SymbolEntry &SE, const LLVMValueMap &VM) const {
    if (SE.IsConstant()) {
      return VM.getValue(SE.AsConstant());
    }
    else if (SE.IsArray()) {
      return VM.getValue(SE.GetScope(), SE.GetName(), SE.AsArray());
    }
    else if (SE.IsVariable()) {
      return VM.getValue(SE.GetScope(), SE.GetName(), SE.AsVariable());
    }
    else if (SE.IsFunction()) {
      return VM.getValue(SE.GetName(), SE.AsFunction());
    }
    llvm_unreachable("Unknown SymbolEntry type");
  }

public:
  LLVMLocalValueTable(SymbolTableView Local, LLVMContext &Context, Module &TheModule)
    : NamedValues() {
     LLVMValueMap ValueMap(TheModule, Context);
    for (const std::pair<String, SymbolEntry> &item : Local) {
      NamedValues.emplace(item.first, ValueFromSymbolEntry(item.second, ValueMap));
    }
  }

  Value *getValue(const String &Name) const {
    assert(NamedValues.count(Name) && "Undefined Name");
    return NamedValues.find(Name)->second;
  }

};

class LLVMIRCompiler : public VisitorBase<LLVMIRCompiler> {
  /// llvm specific members
  LLVMContext TheContext;
  Module TheModule;
  IRBuilder<> Builder;
  std::unordered_map<String, AllocaInst*> NamedValues;
  SymbolTableView LocalTable;

  const SymbolTable &ST;
  ErrorManager Err;
  /* SymbolTableView local; */

  /* void SetLocal(FuncDef *FD) { */
  /*   local = ST.GetLocal(FD); */
  /* } */

  /// Convert a BasicTypeKind to corresponding llvm Type
  Type *LLVMTypeFromBasicKindType(BasicTypeKind T) {
    switch (T) {
    case BasicTypeKind::Character:
      return Type::getInt8Ty(TheContext);
    case BasicTypeKind::Int:
      return Type::getInt32Ty(TheContext);
    case BasicTypeKind::Void:
      return Type::getVoidTy(TheContext);
    }
  }

  /// Convert a FuncDef node to corresponding llvm FunctionType
  FunctionType *LLVMFunctionTypeFromFuncDef(FuncDef *node) {
    auto &&RT = LLVMTypeFromBasicKindType(node->return_type);
    std::vector<Type *> ArgTypes(node->args.size());
    for (int i = 0, e = node->args.size(); i != e; ++i) {
      ArgTypes[i] = LLVMTypeFromBasicKindType(node->args[i]->type);
    }
    return FunctionType::get(RT, ArgTypes, false);
  }

  Value *I32FromInt(int I) {
    return ConstantInt::get(TheContext, llvm::APInt(32, I));
  }

  Value *I8FromInt(int I) {
    return ConstantInt::get(TheContext, APInt(8, I));
  }

public:
  void visitStmt(Stmt *s) { return VisitorBase::visitStmt<void>(s); }

  void visitDecl(Decl *node) { VisitorBase::visitDecl<void>(node); }

  void visitArg(Arg *node) {
  }

  Value *visitExpr(Expr *node) {
    return VisitorBase::visitExpr<Value *>(node);
  }

  Value *visitNum(Num *node) { return I32FromInt(node->n); }

  Value *visitChar(Char *node) { return I8FromInt(node->c); }

  Value *visitBinOp(BinOp *node) {
    auto L = visitExpr(node->left);
    auto R = visitExpr(node->right);
    assert(L && R);
    switch (node->op) {
    case OperatorKind::Add:
      return Builder.CreateAdd(L, R, "addtmp");
    case OperatorKind::Sub:
      return Builder.CreateSub(L, R, "subtmp");
    case OperatorKind::Mult:
      return Builder.CreateMul(L, R, "multmp");
    case OperatorKind::Div:
      return Builder.CreateUDiv(L, R, "udivtmp");
    case OperatorKind::Eq:
      return Builder.CreateICmpEQ(L, R, "eqtmp");
    case OperatorKind::NotEq:
      return Builder.CreateICmpNE(L, R, "netmp");
    case OperatorKind::Lt:
      return Builder.CreateICmpSLT(L, R, "slttmp");
    case OperatorKind::LtE:
      return Builder.CreateICmpSLE(L, R, "sletmp");
    case OperatorKind::Gt:
      return Builder.CreateICmpSGT(L, R, "sgttmp");
    case OperatorKind::GtE:
      return Builder.CreateICmpSGE(L, R, "sgetmp");
    }
  }

  Value *visitUnaryOp(UnaryOp *node) {
    auto &&Operand = visitExpr(node->operand);
    switch (node->op) {
    case UnaryopKind::USub:
      return Builder.CreateNeg(Operand, "negtmp");
    case UnaryopKind::UAdd:
      return Operand;
    }
  }

  Value *visitParenExpr(ParenExpr *node) {
    return visitExpr(node->value);
  }

  Value *visitBoolOp(BoolOp *node) { return visitExpr(node->value); }

  void visitIf(If *node) {
    Value *CondV = visitExpr(node->test);
    CondV = Builder.CreateICmpNE(
        CondV, ConstantInt::get(TheContext, APInt(1, 0)), "ifcond");
    Function *TheFunction = Builder.GetInsertBlock()->getParent();

    BasicBlock *ThenBB = BasicBlock::Create(TheContext, "then", TheFunction);
    BasicBlock *ElseBB = BasicBlock::Create(TheContext, "else");
    BasicBlock *MergeBB = BasicBlock::Create(TheContext, "ifcont");

    Builder.CreateCondBr(CondV, ThenBB, ElseBB);
    Builder.SetInsertPoint(ThenBB);
    for (auto &&S : node->body) {
      visitStmt(S);
    }
    Builder.CreateBr(MergeBB);

    TheFunction->getBasicBlockList().push_back(ElseBB);
    Builder.SetInsertPoint(ElseBB);
    for (auto &&S : node->orelse) {
      visitStmt(S);
    }
    Builder.CreateBr(MergeBB);

    TheFunction->getBasicBlockList().push_back(MergeBB);
  }

  void visitWhile(While *node) {
    auto &&CondV = visitExpr(node->condition);
    auto &&BodyBB = BasicBlock::Create(TheContext, "body");
    auto &&EndBB = BasicBlock::Create(TheContext, "endWhile");
    Builder.CreateCondBr(CondV, BodyBB, EndBB);
    Builder.SetInsertPoint(BodyBB);
    for (auto &&S : node->body) {
      visitStmt(S);
    }
    Builder.SetInsertPoint(EndBB);
  }

  /* Value *visitName(Name *node) { */
  /*   auto &&Entry = local[node->id]; */
  /*   if (Entry.IsConstant()) { */
  /*     auto &&CT = Entry.AsConstant(); */
  /*     return CT.GetType() == BasicTypeKind::Int ? I32FromInt(CT.GetValue())
   */
  /*                                               : I8FromInt(CT.GetValue());
   */
  /*   } */

  /* } */

  Value *visitCall(Call *node) {
    Value *Callee = TheModule.getFunction(node->func);
    assert(Callee && "Callee must be a created Function");

    std::vector<Value *> ArgsV;
    ArgsV.reserve(node->args.size());
    for (Expr *A: node->args) {
      Value *Val = visitExpr(A);
      ArgsV.push_back(Val);
    }

    return Builder.CreateCall(Callee, ArgsV, "calltmp");
  }

  void visitReturn(Return *node) {
    if (node->value) {
      Value *Val = visitExpr(node->value);
      Builder.CreateRet(Val);
    } else {
      Builder.CreateRetVoid();
    }
  }

  Value *visitStr(Str *S) {
    /* return ConstantDataArray::getString(TheContext, S->s); */
  }

  Value *visitName(Name *N) {
    return nullptr;
  }

  void visitAssign(Assign *A) {

  }

  Value *visitSubscript(Subscript *SB) {
    return nullptr;
  }

  void visitExprStmt(ExprStmt *ES) {}

  void visitRead(Read *RD) {
    /// Emit a scanf("%c", &Var) or scanf("%d", &Var)
    /// depending on the type of Var.
    Function *Scanf = TheModule.getFunction("scanf");
    assert(Scanf && "scanf must be declared");
    std::vector<Value*> Args(2);

    for (Expr *E : RD->names) {
      Name *Nn = static_cast<Name*>(E);
      Value *Var = NamedValues[Nn->id];
      assert(Var && "Var must be created");
      // do some check here
      const char *Fmt = nullptr;
      if (Var->getType() == Type::getInt8Ty(TheContext)) {
        Fmt = "%c";
      }
      else {
        assert(Var->getType() == Type::getInt32Ty(TheContext));
        Fmt = "%d";
      }
      Args.clear();
      Args.push_back(ConstantDataArray::getString(TheContext, Fmt));
      Args.push_back(Var);
      Builder.CreateCall(Scanf, Args, "readtmp");
    }
  }

  void visitWrite(Write *WR) {
    /// Emit a printf("%

  }

  void visitVarDecl(VarDecl *VD) {


  }

  void visitConstDecl(ConstDecl *CD) {

  }

  void visitFor(For *F) {


  }

  void visitFuncDef(FuncDef *node) {
    /* SetLocal(node); */
    /// Build the FunctionType
    auto &&FT = LLVMFunctionTypeFromFuncDef(node);
    auto &&F = Function::Create(FT, Function::ExternalLinkage,
                                      node->name, TheModule);

    /// Create entry point
    auto &&BB = BasicBlock::Create(TheContext, "entry", F);
    Builder.SetInsertPoint(BB);

    for (auto &&Arg : node->args) {
      visitArg(Arg);
    }
    for (auto &&Decl : node->decls) {
      visitDecl(Decl);
    }
    for (auto &&Stmt : node->stmts) {
      visitStmt(Stmt);
    }

    /// Verify the function body
    String ErrorMsg;
    llvm::raw_string_ostream OS(ErrorMsg);
    if (llvm::verifyFunction(*F, &OS)) {
      Err.Error(ErrorMsg);
      F->eraseFromParent();
    }
  }

public:
  LLVMIRCompiler(const SymbolTable &ST)
      : TheContext(), TheModule("simplecompiler", TheContext), Builder(TheContext), ST(ST),
        Err() {}
};

} // namespace
