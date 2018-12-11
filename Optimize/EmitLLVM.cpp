#include "EmitLLVM.h"
#include "ErrorManager.h"
#include "SymbolTable.h"
#include "Visitor.h"

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

#include <system_error>
#include <unordered_map>
#include <vector>

namespace simplecompiler {
using llvm::AllocaInst;
using llvm::BasicBlock;
using llvm::Constant;
using llvm::ConstantDataArray; /// For string literal
using llvm::ConstantInt;
using llvm::Function;
using llvm::FunctionType;
using llvm::GlobalVariable;
using llvm::Instruction;
using llvm::IRBuilder;
using llvm::LLVMContext;
using llvm::Module;
using llvm::StringRef;
using llvm::Type;
using llvm::Value;

/// A class that translates simplecompiler's type system to LLVM's type system.
/// Provides helpers to convert objects of different types to their LLVM
/// counterparts.
class LLVMTypeMap {
  LLVMContext &TheContext;

protected:
  LLVMContext &getContext() const { return TheContext; }

public:
  LLVMTypeMap(LLVMContext &Context) : TheContext(Context) {}

  /// Convert a basic type.
  Type *getType(BasicTypeKind Type) const {
    switch (Type) {
    case BasicTypeKind::Character:
      return Type::getInt8Ty(TheContext);
    case BasicTypeKind::Int:
      return Type::getInt32Ty(TheContext);
    case BasicTypeKind::Void:
      return Type::getVoidTy(TheContext);
    }
  }

  Type *getTypeFromVarDecl(VarDecl *VD) const {
    if (VD->is_array)
      return getType(ArrayType(VD));
    return getType(VarType(VD->type));
  }

  /// Convert an ArrayType.
  Type *getType(const ArrayType &A) const {
    return llvm::ArrayType::get(getType(A.GetElementType()), A.GetSize());
  }

  /// Convert a VarType.
  Type *getType(const VarType &V) const { return getType(V.GetType()); }

  /// Convert a FuncType.
  FunctionType *getType(const FuncType &F) const {
    Type *ReturnType = getType(F.GetReturnType());
    std::vector<Type *> ArgTypes(F.GetArgCount());
    for (int i = 0; i < F.GetArgCount(); i++) {
      ArgTypes[i] = getType(F.GetArgTypeAt(i));
    }
    return FunctionType::get(ReturnType, ArgTypes, false);
  }

  Type *getType(const ConstType &C) const { return getType(C.GetType()); }

  Type *getTypeFromConstDecl(ConstDecl *CD) const { return getType(CD->type); }
};

/// A class that convert values of different types to LLVM counterparts.
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

  /// Convert a Constant value.
  Value *getConstant(const ConstType &C) const {
    return ConstantInt::get(getType(C.GetType()), C.GetValue(), true);
  }

  /// Convert a Variable value.
  Value *getVariable(Scope S, const String &Name, const VarType &V) const {
    switch (S) {
    case Scope::Global:
      return getValueGlobal(Name, V);
    case Scope::Local:
      return getValueLocal(Name, V);
    }
  }

  /// Convert an Array value.
  Value *getArray(Scope S, const String &Name, const ArrayType &V) const {
    switch (S) {
    case Scope::Global:
      return getValueGlobal(Name, V);
    case Scope::Local:
      return getValueLocal(Name, V);
    }
  }

  /// Convert a Function value. Only prototype is created.
  Value *getFunction(const String &Name, const FuncType &F) const {
    Type *ReturnType = getType(F.GetReturnType());
    std::vector<Type *> ArgTypes(F.GetArgCount());
    for (int i = 0; i < F.GetArgCount(); i++) {
      ArgTypes[i] = getType(F.GetArgTypeAt(i));
    }
    FunctionType *FT = FunctionType::get(ReturnType, ArgTypes, false);
    return TheModule.getOrInsertFunction(Name, FT);
  }

  Value *getFunction(FuncDef *FD) const {
    return getFunction(FD->name, FuncType(FD));
  }

  /// Convert a Num node to int value.
  Constant *getInt(int N) const {
    return ConstantInt::get(getType(BasicTypeKind::Int), N, false);
  }

  /// Convert a Char node to char value.
  Constant *getChar(char C) const {
    return ConstantInt::get(getType(BasicTypeKind::Character), C, false);
  }

  // Convert a constant expression.
  Constant *getConstantFromExpr(Expr *E) const {
    switch (E->GetKind()) {
    case Expr::Num:
      return getInt(static_cast<Num *>(E)->n);
    case Expr::Char:
      return getChar(static_cast<Char *>(E)->c);
    default:
      llvm_unreachable("Expr must be Num or Char");
    }
  }

  /// Convert a bool value.
  Constant *getBool(bool B) const {
    return B ? ConstantInt::getTrue(getContext())
             : ConstantInt::getFalse(getContext());
  }

  Constant *getGlobalInitializer(VarDecl *VD) {
    if (VD->is_array) {
      return llvm::ConstantAggregateZero::get(getType(VD));
    }
    switch (VD->type) {
    case BasicTypeKind::Int:
      return getInt(0);
    case BasicTypeKind::Character:
      return getChar(0);
    default:
      llvm_unreachable("Void cannot be");
    }
  }
};

/// A class that emits LLVM IR from an AST. This class concretely
/// implements the LLVM IR code generation.
class LLVMIRCompilerImpl : VisitorBase<LLVMIRCompilerImpl> {

  /// Declare builtin functions (external really, but let's call it builtin).
  void DeclareBuiltinFunctions() {
    /* declare i32 @printf(i8*, ...) */
    DeclareIOBuiltins("printf");
    /* declare i32 @__isoc99_scanf(i8*, ...) */
    DeclareIOBuiltins("scanf");
  }

  /// Helper of DeclareBuiltinFunctions() since their prototypes is similar.
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

  Value *getString(StringRef Str) { return Builder.CreateGlobalStringPtr(Str); }

  /// VisitorBase boilderplate.
  void visitStmt(Stmt *s) { return VisitorBase::visitStmt<void>(s); }
  void visitDecl(Decl *node) { VisitorBase::visitDecl<void>(node); }
  Value *visitExpr(Expr *E) { return VisitorBase::visitExpr<Value *>(E); }

  /// Simple atom nodes.
  Value *visitNum(Num *N) { return VM.getInt(N->n); }
  Value *visitChar(Char *C) { return VM.getChar(C->c); }
  Value *visitStr(Str *S) { return getString(S->s); }

  Value *visitName(Name *Nn) {
    Value *Ptr = LocalValues_[Nn->id];
    assert(Ptr);
    if (Nn->ctx == ExprContextKind::Load) {
      return Builder.CreateLoad(Ptr, Nn->id);
    }
    return Ptr;
  }

  /// Simple wrapper nodes.
  Value *visitParenExpr(ParenExpr *PE) { return visitExpr(PE->value); }
  void visitExprStmt(ExprStmt *ES) { visitExpr(ES->value); }

  /// Convert the condition value to non-zeroness.
  Value *visitBoolOp(BoolOp *B) {
    Value *Val = visitExpr(B->value);
    return Builder.CreateICmpNE(Val, VM.getBool(false), "condtmp");
  }

  Value *visitBinOp(BinOp *B) {
    Value *L = visitExpr(B->left);
    Value *R = visitExpr(B->right);
    assert(L && R);
    switch (B->op) {
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

  Value *visitUnaryOp(UnaryOp *U) {
    Value *Operand = visitExpr(U->operand);
    switch (U->op) {
    case UnaryopKind::USub:
      return Builder.CreateNeg(Operand, "negtmp");
    case UnaryopKind::UAdd:
      return Operand;
    }
  }

  void visitIf(If *I) {
    Function *TheFunction = Builder.GetInsertBlock()->getParent();
    /// Emit the condition evalation, which continues in the current BasicBlock.
    Value *CondV = visitExpr(I->test);
    /// Create the targets for the conditional branch that ends the current
    /// BasicBlock.
    BasicBlock *Then = BasicBlock::Create(TheContext, "then", TheFunction);
    BasicBlock *Else = BasicBlock::Create(TheContext, "else", TheFunction);
    BasicBlock *End = BasicBlock::Create(TheContext, "end", TheFunction);
    /// If true, goto Then; else goto Else (Always here).
    Builder.CreateCondBr(CondV, Then, Else);

    /// Begin to emit the body into Then BB.
    Builder.SetInsertPoint(Then);
    for (Stmt *S : I->body) {
      visitStmt(S);
    }
    /// Ends with an unconditional branch to End.
    Builder.CreateBr(End);

    /// Begin to emit the orelse into Else BB.
    Builder.SetInsertPoint(Else);
    for (Stmt *S : I->orelse) {
      visitStmt(S);
    }
    /// Ends with an unconditional branch to End.
    Builder.CreateBr(End);

    Builder.SetInsertPoint(End);
  }

  void visitWhile(While *W) {
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
    Value *CondV = visitExpr(W->condition);

    /// Create the targets of a conditional branch that ends loop BB.
    BasicBlock *End = BasicBlock::Create(TheContext, "end", TheFunction);
    BasicBlock *Body = BasicBlock::Create(TheContext, "body", TheFunction);
    /// If true, goto the BodyBB, else goto the EndBB.
    Builder.CreateCondBr(CondV, Body, End);

    /// Begin to emit the Body, which is ``while { body }``.
    Builder.SetInsertPoint(Body);
    for (Stmt *S : W->body) {
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
  void visitFor(For *F) {
    Function *TheFunction = Builder.GetInsertBlock()->getParent();
    /// Create all the BasicBlock's involved all at once.
    BasicBlock *Body = BasicBlock::Create(TheContext, "body", TheFunction);
    BasicBlock *Loop = BasicBlock::Create(TheContext, "loop", TheFunction);
    BasicBlock *End = BasicBlock::Create(TheContext, "end", TheFunction);

    /// Execute initial stmt in the current BB.
    visitStmt(F->initial);
    /// End of initial BB: Immediately jump to the Body.
    Builder.CreateBr(Body);

    /// Begin Loop: step; condition => (Body, End).
    Builder.SetInsertPoint(Loop);
    visitStmt(F->step);
    Value *CondV = visitExpr(F->condition);
    /// End of Loop
    Builder.CreateCondBr(CondV, /* true */ Body, /* false */ End);

    /// Begin Body:
    Builder.SetInsertPoint(Body);
    for (Stmt *S : F->body) {
      visitStmt(S);
    }
    /// End of Body: jump back to Loop.
    Builder.CreateBr(Loop);

    /// End of For.
    Builder.SetInsertPoint(End);
  }

  Value *visitCall(Call *C) {
    Value *Callee = LocalValues_[C->func];
    assert(Callee && "Callee must be created");
    std::vector<Value *> ArgsV;
    ArgsV.reserve(C->args.size());
    for (Expr *A : C->args) {
      Value *Val = visitExpr(A);
      ArgsV.push_back(Val);
    }
    return Builder.CreateCall(Callee, ArgsV, "calltmp");
  }

  void visitReturn(Return *Ret) {
    if (Ret->value) {
      Value *Val = visitExpr(Ret->value);
      Builder.CreateRet(Val);
    } else {
      Builder.CreateRetVoid();
    }
    /// Return is a Terminator. End this BB and create a new one.
    /// Normally one should not write code after a return because these
    /// code obviously dead (without goto). But is is possible and a good chance
    /// for DCE.
    auto TheFunction = Builder.GetInsertBlock()->getParent();
    BasicBlock *NextBB = BasicBlock::Create(TheContext, "next", TheFunction);
    Builder.SetInsertPoint(NextBB);
  }

  void visitAssign(Assign *A) {
    Value *RHS = visitExpr(A->value);
    Value *LHS = visitExpr(A->target);
    Builder.CreateStore(RHS, LHS);
  }

  /// The logic varies depending on whether it is a load/store and whether
  /// it is a global/local array.
  Value *visitSubscript(Subscript *SB) {
    Value *Array = LocalValues_[SB->name];

    assert(Array && "Array Value must exist");
    Value *Index = visitExpr(SB->index);

    /// Always remember Array values are represented by **ptr to array**
    /// and to get an address to its element, it **must** be stepped through
    /// first using a zero index in getelementptr and then the desired index.
    Value *IdxList[2] = {VM.getInt(0), Index};
    Value *ElemPtr = Builder.CreateInBoundsGEP(Array, IdxList, "subscr");

    switch (SB->ctx) {
    case ExprContextKind::Load:
      /// If this is a Load, emit a load.
      return Builder.CreateLoad(ElemPtr, "elemtmp");
    case ExprContextKind::Store:
      /// If this is a Store, just return the ptr to the elememt
      /// to be stored by an Assign.
      return ElemPtr;
    }
  }

  void visitRead(Read *RD) {
    /// Emit a scanf("%c", &Var) or scanf("%d", &Var)
    /// depending on the type of Var.
    Function *Scanf = TheModule.getFunction("scanf");
    assert(Scanf && "scanf() must be declared");
    llvm::SmallVector<Value *, 2> Args;

    /// Select appropriate format specifier by type.
    auto SelectFmtSpc = [this](Expr *Name) {
      auto T = TheTable.GetExprType(Name);
      switch (T) {
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
      Value *FmtV = getString(SelectFmtSpc(Nn));
      Value *Var = LocalValues_[Nn->id];
      assert(Var && "Var must be created");
      Args.clear();
      Args.push_back(FmtV);
      Args.push_back(Var);
      Builder.CreateCall(Scanf, Args, "scanf");
    }
  }

  /// Emit a printf("%d\n", Var) for printf(<int-expr>);
  ///      a printf("%c\n", Var) for printf(<char-expr>);
  ///      a printf("%s\n", Str) for printf(<string>);
  ///      a printf("%s%d\n", Str, Var) for printf(<string>, <int-expr>);
  ///      a printf("%s%c\n", Str, Var) for printf(<string>, <char-expr>);
  void visitWrite(Write *WR) {
    /// A small lambda to select the appropriate format specifier.
    auto SelectFmtSpc = [WR, this]() {
      if (!WR->value) {
        // No expr, no need to consult SymbolTable
        return "%s\n";
      }
      auto T = TheTable.GetExprType(WR->value);
      if (!WR->str) {
        // No string.
        return T == BasicTypeKind::Character ? "%c\n" : "%d\n";
      }
      return T == BasicTypeKind::Character ? "%s%c\n" : "%s%d\n";
    };
    Function *Printf = TheModule.getFunction("printf");
    assert(Printf && "printf() must be declared");

    /// Take a maximum of 3 arguments.
    llvm::SmallVector<Value *, 3> Args;
    Value *FmtV = getString(SelectFmtSpc());
    Args.push_back(FmtV);
    if (WR->str)
      Args.push_back(visitExpr(WR->str));
    if (WR->value)
      Args.push_back(visitExpr(WR->value));
    Builder.CreateCall(Printf, Args);
  }

  /// Generate body for a Function.
  void visitFuncDef(FuncDef *FD) {
    /// Clear the Mapping.
    LocalValues_.clear();

    auto Fn = Function::Create(
        /* FunctionType */ TM.getType(FD),
        /* Linkage */ Function::InternalLinkage,
        /* Name */ FD->name,
        /* Module */ &TheModule);

    /// Create the entry point (Function body).
    BasicBlock *EntryBlock = BasicBlock::Create(TheContext, "entry", Fn);
    Builder.SetInsertPoint(EntryBlock);

    /// Setup arguments.
    for (llvm::Argument &Val : Fn->args()) {
      auto Idx = Val.getArgNo();
      ArgDecl *V = static_cast<ArgDecl *>(FD->args[Idx]);
      Val.setName(V->name);
      /// Argument is never array.
      auto Ptr = Builder.CreateAlloca(TM.getType(V->type), nullptr, V->name);
      /// Store the initial value of an argument.
      Builder.CreateStore(&Val, Ptr);
      LocalValues_.emplace(V->name, Ptr);
    }

    /// Setup alloca for local storage.
    /// Setup local constants.
    for (Decl *D : FD->decls) {
      if (auto VD = subclass_cast<VarDecl>(D)) {
        auto Alloca = Builder.CreateAlloca(
            /* Type */ TM.getType(VD->type),
            /* ArraySize */ VD->is_array ? VM.getInt(VD->size) : nullptr,
            /* Name */ VD->name);
        LocalValues_.emplace(VD->name, Alloca);
      } else if (auto CD = subclass_cast<ConstDecl>(D)) {
        LocalValues_.emplace(CD->name, VM.getConstantFromExpr(CD->value));
      }
    }

    /// Populate LocalValues with global objects.
    SymbolTableView Local = TheTable.GetLocal(FD);
    for (auto &&Pair : Local) {
      const SymbolEntry &E = Pair.second;
      if (E.IsLocal()) {
        assert(LocalValues_.count(E.GetName()) &&
               "Local Decl must have been handled");
        continue;
      }
      if (E.IsFunction()) {
        LocalValues_.emplace(E.GetName(), TheModule.getFunction(E.GetName()));
      } else {
        /// GlobalVariable
        LocalValues_.emplace(E.GetName(),
                             TheModule.getGlobalVariable(E.GetName()));
      }
    }

    /// Generate the body
    for (Stmt *S : FD->stmts) {
      visitStmt(S);
    }

    /// Check for well-formness of all BBs. In particular, look for
    /// any unterminated BB and try to add a Return to it.
    for (BasicBlock &BB : *Fn) {
      Instruction *Terminator = BB.getTerminator();
      if (Terminator != nullptr)
        continue; /// Well-formed
      if (Fn->getReturnType()->isVoidTy()) {
        /// Make implicit return of void Function explicit.
        Builder.SetInsertPoint(&BB);
        Builder.CreateRetVoid();
      } else {
        // How to attach source location?
        EM.Error("control flow reaches end of non-void function");
        // No source location, make errors short
        return;
      }
    }
    /// No need to call verifyFunction() since verifyModule() does that.
  }

  /// Create a global constant.
  void visitConstDecl(ConstDecl *CD) {
    // Create a global constant.
    auto GV = new llvm::GlobalVariable(
        /* Module */ TheModule,
        /* Type */ TM.getTypeFromConstDecl(CD),
        /* IsConstant */ true,
        /* Linkage */ GlobalVariable::InternalLinkage,
        /* Initializer */ VM.getConstantFromExpr(CD->value),
        /* Name */ CD->name);
    (void)GV;
  }

  /// Create a global variable.
  void visitVarDecl(VarDecl *VD) {
    auto GV = new llvm::GlobalVariable(
        /* Module */ TheModule,
        /* Type */ TM.getTypeFromVarDecl(VD),
        /* IsConstant */ false,
        /* Linkage */ GlobalVariable::InternalLinkage,
        /* Initializer */ VM.getGlobalInitializer(VD),
        /* Name */ VD->name);
    (void)GV;
  }

  void visitArgDecl(ArgDecl *) {}

  void visitProgram(Program *P) {
    for (Decl *D : P->decls) {
      visitDecl(D);
    }
    /// Verify the Module.
    String ErrorMsg;
    llvm::raw_string_ostream OS(ErrorMsg);
    if (llvm::verifyModule(TheModule, &OS)) {
      EM.Error(ErrorMsg);
    }
  }

public:
  LLVMIRCompilerImpl(const SymbolTable &S, llvm::LLVMContext &C,
                     llvm::Module &M)
      : TheTable(S), TheContext(C), TheModule(M), Builder(C), EM(), TM(C),
        VM(M, C) {
    DeclareBuiltinFunctions();
  }

  bool Compile(Program *P) {
    visitProgram(P);
    return EM.IsOk();
  }

  LLVMIRCompilerImpl(const LLVMIRCompilerImpl &) = delete;
  LLVMIRCompilerImpl(LLVMIRCompilerImpl &&) = delete;

private:
  friend class VisitorBase<LLVMIRCompilerImpl>;

  /// Used for getting types of an Expr.
  const SymbolTable &TheTable;
  SymbolTableView LocalTable;

  /// Keep a reference to core LLVM data structures
  LLVMContext &TheContext;
  llvm::Module &TheModule;

  /// Major tool for emitting instructions.
  IRBuilder<> Builder;

  LLVMValueMap VM;
  LLVMTypeMap TM;
  std::unordered_map<String, Value *> LocalValues_;

  ErrorManager EM;
};

/// Wrapper around LLVMIRCompilerImpl to provide a clearer interface.
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

  /// Compile the program, return true for success.
  bool Compile() { return Impl.Compile(TheProgram); }

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

} // namespace simplecompiler

namespace simplecompiler {

/// Compile a program to LLVM IR, dump resultant code to stderr.
/// Return true for success.
bool CompileToLLVMIR(String InputFilename, Program *P, const SymbolTable &S,
                     String OutputFilename) {
  /// Currently no name for a Module.
  LLVMCompiler LC(InputFilename, S, P);

  /// Compile to llvm::Module, fail fast.
  bool OK = LC.Compile();
  if (!OK)
    return false;

  /// Try to open the output file, fail fast.
  std::error_code EC;
  /// raw_fd_ostream treat "-" specially as opening stdout.
  /// We honor its behavior.
  if (OutputFilename.empty())
    OutputFilename = "-";
  llvm::raw_fd_ostream OS(OutputFilename, EC);
  if (EC) {
    llvm::errs() << EC.message() << "\n";
    return false;
  }

  /// Write out the human-readable bitcode.
  LC.getModule().print(OS, nullptr);
  return true;
}

} // namespace simplecompiler
