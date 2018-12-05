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
using llvm::AllocaInst;
using llvm::APInt;
using llvm::BasicBlock;
using llvm::Constant;
using llvm::ConstantDataArray; /// For string literal
using llvm::ConstantInt;
using llvm::Function;
using llvm::FunctionType;
using llvm::IRBuilder;
using llvm::LLVMContext;
using llvm::Module;
using llvm::Type;
using llvm::Value;

/// A class that translates simplecompiler's type system to LLVM's type system.
class LLVMTypeMap {
  LLVMContext &TheContext;

protected:
  LLVMContext &getContext() const { return TheContext; }

public:
  LLVMTypeMap(LLVMContext &Context) : TheContext(Context) {}

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

  Type *getType(const VarType &V) const { return getType(V.GetType()); }

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
  LLVMValueMap(llvm::Module &M, LLVMContext &Context)
      : LLVMTypeMap(Context), TheModule(M) {}

  Value *getConstant(const ConstType &C) const {
    return ConstantInt::get(getType(C.GetType()), C.GetValue(), true);
  }

  Value *getVariable(Scope S, const String &Name, const VarType &V) const {
    switch (S) {
    case Scope::Global:
      return getValueGlobal(Name, V);
    case Scope::Local:
      return getValueLocal(Name, V);
    }
  }

  Value *getArray(Scope S, const String &Name, const ArrayType &V) const {
    switch (S) {
    case Scope::Global:
      return getValueGlobal(Name, V);
    case Scope::Local:
      return getValueLocal(Name, V);
    }
  }

  Value *getFunction(const String &Name, const FuncType &F) const {
    Type *ReturnType = getType(F.GetReturnType());
    std::vector<Type *> ArgTypes(F.GetArgCount());
    for (int i = 0; i < F.GetArgCount(); i++) {
      ArgTypes[i] = getType(F.GetArgTypeAt(i));
    }
    FunctionType *FT = FunctionType::get(ReturnType, ArgTypes, false);
    return TheModule.getOrInsertGlobal(Name, FT);
  }

  Value *getInt(Num *N) const {
    return ConstantInt::get(getType(BasicTypeKind::Int), N->n, false);
  }

  Value *getChar(Char *C) const {
    return ConstantInt::get(getType(BasicTypeKind::Character), C->c, false);
  }

  Value *getString(Str *S) const {
    return ConstantDataArray::getString(getContext(), S->s);
  }

  Value *getString(const char *S) const {
    return ConstantDataArray::getString(getContext(), S);
  }

  Value *getBool(bool B) const {
    return B ? ConstantInt::getTrue(getContext())
             : ConstantInt::getFalse(getContext());
  }
};

/// A class that translates one SymbolTableView of names to their LLVM Values.
class LLVMLocalValueTable {
  std::unordered_map<String, Value *> NamedValues;

  Value *ValueFromSymbolEntry(const SymbolEntry &SE,
                              const LLVMValueMap &VM) const {
    if (SE.IsConstant()) {
      return VM.getConstant(SE.AsConstant());
    } else if (SE.IsArray()) {
      return VM.getArray(SE.GetScope(), SE.GetName(), SE.AsArray());
    } else if (SE.IsVariable()) {
      return VM.getVariable(SE.GetScope(), SE.GetName(), SE.AsVariable());
    } else if (SE.IsFunction()) {
      return VM.getFunction(SE.GetName(), SE.AsFunction());
    }
    llvm_unreachable("Unknown SymbolEntry type");
  }

public:
  LLVMLocalValueTable(SymbolTableView Local, LLVMContext &Context,
                      Module &TheModule)
      : NamedValues() {
    LLVMValueMap ValueMap(TheModule, Context);
    for (const std::pair<String, SymbolEntry> &item : Local) {
      NamedValues.emplace(item.first,
                          ValueFromSymbolEntry(item.second, ValueMap));
    }
  }

  Value *getValue(const String &Name) const {
    assert(NamedValues.count(Name) && "Undefined Name");
    return NamedValues.find(Name)->second;
  }
};

class LLVMIRCompilerImpl : public VisitorBase<LLVMIRCompilerImpl> {
  const SymbolTable &Symbols;

  LLVMContext &TheContext;
  llvm::Module &TheModule;
  IRBuilder<> Builder;

  LLVMValueMap ValueMap;
  /// This mapping changes from function to function.
  std::unique_ptr<LLVMLocalValueTable> LocalValues;
  ErrorManager EM;

  void DeclareBuiltinFunctions() {
    DeclareIOBuiltins("printf");
    DeclareIOBuiltins("scanf");
  }

  /* declare i32 @printf(i8*, ...) */
  /* declare i32 @__isoc99_scanf(i8*, ...) */
  Function *DeclareIOBuiltins(const char *Name) {
    FunctionType *FT = FunctionType::get(
        /* Result */ Type::getInt32PtrTy(TheContext),
        /* Params */ Type::getInt8PtrTy(TheContext),
        /* IsVarArg */ true);
    Function *Fn = Function::Create(
        /* FunctionType */ FT,
        /* Linkage */ Function::ExternalLinkage,
        /* AddressSpace */ 0,
        /* Name */ Name,
        /* Module */ &TheModule);
    return Fn;
  }

public:
  /// VisitorBase boilderplate.
  void visitStmt(Stmt *s) { return VisitorBase::visitStmt<void>(s); }
  void visitDecl(Decl *node) { VisitorBase::visitDecl<void>(node); }
  Value *visitExpr(Expr *E) { return VisitorBase::visitExpr<Value *>(E); }

  void visitConstDecl(ConstDecl *CD) {}
  void visitVarDecl(VarDecl *VD) {}

  /// Simple atom nodes.
  Value *visitNum(Num *N) { return ValueMap.getInt(N); }
  Value *visitChar(Char *C) { return ValueMap.getChar(C); }
  Value *visitStr(Str *S) { return ValueMap.getString(S); }
  Value *visitName(Name *Nn) {
    Value *Ptr = LocalValues->getValue(Nn->id);
    if (Nn->ctx == ExprContextKind::Load) {
      return Builder.CreateLoad(Ptr, Nn->id);
    }
    return Ptr;
  }

  /// Simple wrapper nodes.
  Value *visitParenExpr(ParenExpr *node) { return visitExpr(node->value); }
  void visitExprStmt(ExprStmt *ES) { visitExpr(ES->value); }

  /// Convert the condition value to non-zeroness.
  Value *visitBoolOp(BoolOp *node) {
    Value *Val = visitExpr(node->value);
    return Builder.CreateICmpNE(Val, ValueMap.getBool(false), "condtmp");
  }

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

  void visitIf(If *node) {
    Function *TheFunction = Builder.GetInsertBlock()->getParent();
    /// Emit the condition evalation, which continues in the current BasicBlock.
    Value *CondV = visitExpr(node->test);
    /// Create the targets for the conditional branch that ends the current
    /// BasicBlock.
    BasicBlock *Then = BasicBlock::Create(TheContext, "then", TheFunction);
    BasicBlock *Else = BasicBlock::Create(TheContext, "else", TheFunction);
    BasicBlock *End = BasicBlock::Create(TheContext, "end", TheFunction);
    /// If true, goto Then; else goto Else (Always here).
    Builder.CreateCondBr(CondV, Then, Else);

    /// Begin to emit the body into Then BB.
    Builder.SetInsertPoint(Then);
    for (Stmt *S : node->body) {
      visitStmt(S);
    }
    /// Ends with an unconditional branch to End.
    Builder.CreateBr(End);

    /// Begin to emit the orelse into Else BB.
    Builder.SetInsertPoint(Else);
    for (Stmt *S : node->orelse) {
      visitStmt(S);
    }
    /// Ends with an unconditional branch to End.
    Builder.CreateBr(End);

    Builder.SetInsertPoint(End);
  }

  void visitWhile(While *node) {
    /// Get the Parent to put BasicBlock's in it.
    Function *TheFunction = Builder.GetInsertBlock()->getParent();
    /// Create the BB for `loop`, which contains evalation the condition
    // and ends with a conditional branch.
    BasicBlock *Loop = BasicBlock::Create(TheContext, "loop", TheFunction);
    /// Create an unconditional branch to the loop in the current BB.
    Builder.CreateBr(Loop);
    /// Begin to emit instructions of the loop BB.
    Builder.SetInsertPoint(Loop);
    /// Emit the condition evalation.
    Value *CondV = visitExpr(node->condition);

    /// Create the targets of a conditional branch that ends loop BB.
    BasicBlock *End = BasicBlock::Create(TheContext, "end", TheFunction);
    BasicBlock *Body = BasicBlock::Create(TheContext, "body", TheFunction);
    /// If true, goto the BodyBB, else goto the EndBB.
    Builder.CreateCondBr(CondV, Body, End);

    /// Begin to emit the Body, which is ``while { body }``.
    Builder.SetInsertPoint(Body);
    for (Stmt *S : node->body) {
      visitStmt(S);
    }
    /// The body ends with an unconditional branch to the beginning of loop.
    Builder.CreateBr(Loop);

    /// While ends here. subsequent instructions go by.
    Builder.SetInsertPoint(End);
  }

  /// For is the most complicated beast, and with the requirement to
  /// execute the body **before** the evaluation of condition at first
  /// make it no like ordinary for.
  void visitFor(For *node) {
    Function *TheFunction = Builder.GetInsertBlock()->getParent();
    /// Create all the BasicBlock's involved all at once.
    BasicBlock *Body = BasicBlock::Create(TheContext, "body", TheFunction);
    BasicBlock *Loop = BasicBlock::Create(TheContext, "loop", TheFunction);
    BasicBlock *End = BasicBlock::Create(TheContext, "end", TheFunction);

    /// Execute initial stmt in the current BB.
    visitStmt(node->initial);
    /// End of initial BB: Immediately jump to the Body.
    Builder.CreateBr(Body);

    /// Begin Loop: step; condition => (Body, End).
    Builder.SetInsertPoint(Loop);
    visitStmt(node->step);
    Value *CondV = visitExpr(node->condition);
    /// End of Loop
    Builder.CreateCondBr(CondV, /* true */ Body, /* false */ End);

    /// Begin Body:
    Builder.SetInsertPoint(Body);
    for (Stmt *S : node->body) {
      visitStmt(S);
    }
    /// End of Body: jump back to Loop.
    Builder.CreateBr(Loop);

    /// End of For.
    Builder.SetInsertPoint(End);
  }

  Value *visitCall(Call *node) {
    Value *Callee = LocalValues->getValue(node->func);
    assert(Callee && "Callee must be a created Function");

    std::vector<Value *> ArgsV;
    ArgsV.reserve(node->args.size());
    for (Expr *A : node->args) {
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

  void visitAssign(Assign *A) {
    Value *RHS = visitExpr(A->value);
    Value *LHS = visitExpr(A->target);
    Builder.CreateStore(RHS, LHS);
  }

  Value *visitSubscript(Subscript *SB) {
    Value *Array = LocalValues->getValue(SB->name);
    assert(Array && "Array Value must exist");
    Value *Index = visitExpr(SB->index);
    Value *GEP = Builder.CreateGEP(Array, Index, "elemptr");
    if (SB->ctx == ExprContextKind::Load) {
      /// If this is a Load, emit a load.
      return Builder.CreateLoad(GEP, "elemtmp");
    }
    /// If this is a Store, just return the ptr to the elememt
    /// to be stored by an Assign.
    return GEP;
  }

  void visitRead(Read *RD) {
    /// Emit a scanf("%c", &Var) or scanf("%d", &Var)
    /// depending on the type of Var.
    Function *Scanf = TheModule.getFunction("scanf");
    assert(Scanf && "scanf() must be declared");
    llvm::SmallVector<Value *, 2> Args;

    auto SelectFmtSpc = [this](Expr *Name) {
      TypeEntry T = Symbols.GetExprType(Name);
      switch (T.GetType()) {
      case BasicTypeKind::Int:
        return "%d";
      case BasicTypeKind::Character:
        return "%c";
      default:
        llvm_unreachable("Impossible type of Variable");
      }
    };

    for (Expr *E : RD->names) {
      Name *Nn = static_cast<Name *>(E);
      Value *FmtV = ValueMap.getString(SelectFmtSpc(Nn));
      Value *Var = LocalValues->getValue(Nn->id);
      assert(Var && "Var must be created");
      Args.clear();
      Args.push_back(FmtV);
      Args.push_back(Var);
      Builder.CreateCall(Scanf, Args, "readtmp");
    }
  }

  void visitWrite(Write *WR) {
    /// Emit a printf("%d\n", Var) for printf(<int-expr>);
    //       a printf("%c\n", Var) for printf(<char-expr>);
    //       a printf("%s\n", Str) for printf(<string>);
    //       a printf("%s%d\n", Str, Var) for printf(<string>, <int-expr>);
    //       a printf("%s%c\n", Str, Var) for printf(<string>, <char-expr>);

    /// A small lambda to select the appropriate format specifier.
    auto SelectFmtSpc = [WR, this]() {
      if (!WR->value) {
        // No expr, no need to consult SymbolTable
        return "%s\n";
      }
      TypeEntry T = Symbols.GetExprType(WR->value);
      if (!WR->str) {
        // No string.
        return T.GetType() == BasicTypeKind::Character ? "%c\n" : "%d\n";
      }
      return T.GetType() == BasicTypeKind::Character ? "%s%c\n" : "%s%d\n";
    };
    Function *Printf = TheModule.getFunction("printf");
    assert(Printf && "printf() must be declared");
    llvm::SmallVector<Value *, 3> Args;
    Value *FmtV = ValueMap.getString(SelectFmtSpc());
    Args.push_back(FmtV);
    if (WR->str)
      Args.push_back(visitExpr(WR->str));
    if (WR->value)
      Args.push_back(visitExpr(WR->value));
    Builder.CreateCall(Printf, Args, "printf");
  }

  /// Generate body for a Function.
  void visitFuncDef(FuncDef *FD) {
    Function *Fn = llvm::dyn_cast<Function>(LocalValues->getValue(FD->name));
    assert(Fn && "FuncDef must define a Function");
    Fn->setLinkage(Function::InternalLinkage);

    /// Create entry point
    BasicBlock *EntryBlock = BasicBlock::Create(TheContext, "entry", Fn);
    Builder.SetInsertPoint(EntryBlock);

    assert(FD->args.size() == Fn->arg_size());
    for (llvm::Argument &Val : Fn->args()) {
      auto Idx = Val.getArgNo();
      Arg *V = FD->args[Idx];
      Val.setName(V->name);
      auto Ptr = LocalValues->getValue(V->name);
      /// Store the initial value of an argument.
      Builder.CreateStore(&Val, Ptr);
    }

    /// Generate the body
    for (Stmt *S : FD->stmts) {
      visitStmt(S);
    }

    /// Verify the function body
    String ErrorMsg;
    llvm::raw_string_ostream OS(ErrorMsg);
    if (llvm::verifyFunction(*Fn, &OS)) {
      EM.Error(ErrorMsg);
    }
  }

  /// Public interface.
  bool visitProgram(Program *P) {
    for (Decl *D : P->decls) {
      if (auto FD = subclass_cast<FuncDef>(D)) {
        LocalValues = std::make_unique<LLVMLocalValueTable>(
            /* Local */ Symbols.GetLocal(FD),
            /* Context */ TheContext,
            /* Module */ TheModule);
        visitFuncDef(FD);
      }
    }
    return EM.IsOk();
  }

public:
  LLVMIRCompilerImpl(const SymbolTable &S, llvm::LLVMContext &C,
                     llvm::Module &M)
      : Symbols(S), TheContext(C), TheModule(M), Builder(C), ValueMap(M, C),
        LocalValues(), EM() {
    DeclareBuiltinFunctions();
  }

  LLVMIRCompilerImpl(const LLVMIRCompilerImpl &) = delete;
  LLVMIRCompilerImpl(LLVMIRCompilerImpl &&) = delete;
};

class LLVMCompiler {
  llvm::LLVMContext TheContext;
  llvm::Module TheModule;
  Program *TheProgram;
  LLVMIRCompilerImpl Impl;

public:
  LLVMCompiler(const String &Name, const SymbolTable &S, Program *P)
      : TheContext(), TheModule(Name, TheContext), TheProgram(P),
        Impl(S, TheContext, TheModule) {}

  LLVMCompiler(const LLVMCompiler &) = delete;
  LLVMCompiler(LLVMCompiler &&) = delete;

  bool Compile() { return Impl.visitProgram(TheProgram); }

  /// Access the compiled Module.
  llvm::Module &getModule() { return TheModule; }
  const llvm::Module &getModule() const { return TheModule; }

  /// Access the Context.
  llvm::LLVMContext &getContext() { return TheContext; }
  const llvm::LLVMContext &getContext() const { return TheContext; }

  /// Access the Program being compiled.
  const Program *getProgram() const { return TheProgram; }
  Program *getProgram() { return TheProgram; }
};

} // namespace

namespace simplecompiler {

bool CompileToLLVMIR(Program *P, const SymbolTable &S) {
  LLVMCompiler LC("", S, P);
  if (bool Result = LC.Compile(); !Result) {
    return false;
  }
  llvm::Module &M = LC.getModule();
  M.print(llvm::errs(), nullptr);
  return true;
}

}
