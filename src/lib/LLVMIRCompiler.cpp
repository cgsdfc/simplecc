#include "simplecc/LLVMIRCompiler.h"
#include "simplecc/SymbolTable.h"

#include <llvm/IR/Verifier.h>

using namespace simplecc;
using llvm::StringRef;
using llvm::BasicBlock;
using llvm::Instruction;
using llvm::GlobalVariable;

LLVMIRCompiler::LLVMIRCompiler(String Name, Program *P, const SymbolTable &S)
    : TheTable(S), TheProgram(P), TheContext(), TheModule(Name, TheContext),
      Builder(TheContext), VM(TheModule, TheContext), LocalValues(),
      GlobalValues(), EM("LLVMIRError") {
  DeclareBuiltinFunctions();
}

LLVMIRCompiler::~LLVMIRCompiler() = default;

std::unique_ptr<LLVMIRCompiler> LLVMIRCompiler::Create(String Name, Program *P,
                                                       const SymbolTable &S) {
  return std::make_unique<LLVMIRCompiler>(Name, P, S);
}

/// Compile the program, return OK or not.
bool LLVMIRCompiler::Compile() {
  visitProgram(TheProgram);
  return EM.IsOk();
}

/// This helper visits a list of statements, skip those that appear **after**
/// a Return.
/// It returns true if **no Return** is found in between.
/// Client can use its return value to judge whether they should insert
/// a Terminator.
bool LLVMIRCompiler::visitStmtList(const std::vector<Stmt *> &StatementList) {
  for (Stmt *S : StatementList) {
    visitStmt(S);
    if (IsInstance<Return>(S)) {
      /// We see a Return.
      return false;
    }
  }
  /// No Return ever seen.
  return true;
}

Value *LLVMIRCompiler::visitUnaryOp(UnaryOp *U) {
  Value *Operand = visitExprPromoteToInt(U->getOperand());
  switch (U->getOp()) {
  case UnaryopKind::USub:return Builder.CreateNeg(Operand, "neg");
  case UnaryopKind::UAdd:return Operand;
  }
}

/// Declare builtin functions (external really, but let's call it builtin).
void LLVMIRCompiler::DeclareBuiltinFunctions() {
  /* declare i32 @printf(i8*, ...) */
  DeclareIOBuiltins("printf");
  /* declare i32 @__isoc99_scanf(i8*, ...) */
  DeclareIOBuiltins("scanf");
}

/// Helper of DeclareBuiltinFunctions() since their prototypes is similar.
Function *LLVMIRCompiler::DeclareIOBuiltins(const char *Name) {
  FunctionType *FT = FunctionType::get(
      /* Result */ Type::getInt32Ty(TheContext),
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

/// Helper to create IR that loads a string literal (global string ptr).
Value *LLVMIRCompiler::getString(StringRef Str) { return Builder.CreateGlobalStringPtr(Str); }

Value *LLVMIRCompiler::visitStr(Str *S) {
  /// Strip quotes first.
  auto &&Str = S->getS();
  assert(Str.size() >= 2 && "Str must have quotes");
  String Stripped(Str.begin() + 1, Str.end() - 1);
  return getString(Stripped);
}

Value *LLVMIRCompiler::visitName(Name *Nn) {
  Value *Val = LocalValues[Nn->getId()];
  assert(Val);

  /// This is a Store, return its address.
  if (Nn->getCtx() == ExprContextKind::Store) {
    return Val;
  }

  /// This is a variable so **load** it.
  if (!llvm::isa<llvm::ConstantInt>(Val)) {
    Val = Builder.CreateLoad(Val, Nn->getId());
  }

  return Val;
}

/// The tricky part of BoolOp:
/// BoolOp has two forms, indicated by getHasCmpop() and should be handled
/// separatly: The mission of visitBoolOp() is to evaluate the condition
/// expression and produce a bool value that indicates whether the condition
/// is true. We have thoese in grammar: Form-1: <Expr> <RichCompareOp> <Expr>
/// => bool -- already a bool. Form-2: <Expr> => int -- not a bool yet,
/// compare it to int(0).
Value *LLVMIRCompiler::visitBoolOp(BoolOp *B) {
  Value *Val = visitExpr(B->getValue());

  if (Val->getType() == VM.getBoolType()) {
    /// RichCompareOp produces a bool, we cross validate that.
    assert(B->getHasCmpop() && "RichCompareOp must produce a bool");
    /// We already got a bool
    return Val;
  } else if (Val->getType() == VM.getIntType()) {
    return Builder.CreateICmpNE(Val, VM.getInt(0), "cmp");
  } else {
    /// This is impossible. Char must be promoted to int to reach BoolOp.
    llvm_unreachable("Char cannot be!");
  }
}

/// This method accept an int or char and cast it to an int.
Value *LLVMIRCompiler::PromoteToInt(Value *Val) {
  if (Val->getType() == VM.getIntType())
    return Val; /// The same as target type.
  if (Val->getType() == VM.getCharType())
    return Builder.CreateIntCast(Val, VM.getIntType(), /* isSigned */ false,
                                 "cast");
  llvm_unreachable("Val->getType() must be char or int");
}

/// BinOp requires both operands to be int's.
Value *LLVMIRCompiler::visitBinOp(BinOp *B) {
  Value *L = visitExprPromoteToInt(B->getLeft());
  Value *R = visitExprPromoteToInt(B->getRight());
  switch (B->getOp()) {
  case OperatorKind::Add:return Builder.CreateAdd(L, R, "add");
  case OperatorKind::Sub:return Builder.CreateSub(L, R, "sub");
  case OperatorKind::Mult:return Builder.CreateMul(L, R, "mul");
  case OperatorKind::Div:return Builder.CreateSDiv(L, R, "div");
  case OperatorKind::Eq:return Builder.CreateICmpEQ(L, R, "eq");
  case OperatorKind::NotEq:return Builder.CreateICmpNE(L, R, "ne");
  case OperatorKind::Lt:return Builder.CreateICmpSLT(L, R, "lt");
  case OperatorKind::LtE:return Builder.CreateICmpSLE(L, R, "le");
  case OperatorKind::Gt:return Builder.CreateICmpSGT(L, R, "gt");
  case OperatorKind::GtE:return Builder.CreateICmpSGE(L, R, "ge");
  }
}

void LLVMIRCompiler::visitIf(If *I) {
  Function *TheFunction = Builder.GetInsertBlock()->getParent();
  /// Emit the condition evalation, which continues in the current BasicBlock.
  Value *CondV = visitExpr(I->getTest());
  /// Create the targets for the conditional branch that ends the current
  /// BasicBlock.
  BasicBlock *Then = BasicBlock::Create(TheContext, "then", TheFunction);
  BasicBlock *Else = BasicBlock::Create(TheContext, "else", TheFunction);
  BasicBlock *End = BasicBlock::Create(TheContext, "end", TheFunction);
  /// If true, goto Then; else goto Else (Always here).
  Builder.CreateCondBr(CondV, Then, Else);

  /// Begin to emit the body into Then BB.
  Builder.SetInsertPoint(Then);
  /// Check to see if control returns in the middle. If so
  /// don't insert a Br.
  if (visitStmtList(I->getBody())) {
    /// Ends with an unconditional branch to End.
    Builder.CreateBr(End);
  }

  /// Begin to emit the orelse into Else BB.
  Builder.SetInsertPoint(Else);
  if (visitStmtList(I->getOrelse())) {
    /// Ends with an unconditional branch to End.
    Builder.CreateBr(End);
  }

  Builder.SetInsertPoint(End);
}

void LLVMIRCompiler::visitWhile(While *W) {
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
  Value *CondV = visitExpr(W->getCondition());

  /// Create the targets of a conditional branch that ends loop BB.
  BasicBlock *End = BasicBlock::Create(TheContext, "end", TheFunction);
  BasicBlock *Body = BasicBlock::Create(TheContext, "body", TheFunction);
  /// If true, goto the BodyBB, else goto the EndBB.
  Builder.CreateCondBr(CondV, Body, End);

  /// Begin to emit the Body, which is ``while { body }``.
  Builder.SetInsertPoint(Body);
  if (visitStmtList(W->getBody())) {
    /// The body ends with an unconditional branch to the beginning of loop.
    Builder.CreateBr(Loop);
  }

  /// While ends here. subsequent instructions go by.
  Builder.SetInsertPoint(End);
}

/// For is the most complicated beast, and with the requirement to
/// execute the body **before** the evaluation of condition at first
/// make it no like ordinary for.
void LLVMIRCompiler::visitFor(For *F) {
  Function *TheFunction = Builder.GetInsertBlock()->getParent();
  /// Create all the BasicBlock's involved all at once.
  BasicBlock *Body = BasicBlock::Create(TheContext, "body", TheFunction);
  BasicBlock *Loop = BasicBlock::Create(TheContext, "loop", TheFunction);
  BasicBlock *End = BasicBlock::Create(TheContext, "end", TheFunction);

  /// Execute initial stmt in the current BB.
  visitStmt(F->getInitial());
  /// End of initial BB: Immediately jump to the Body.
  Builder.CreateBr(Body);

  /// Begin Loop: step; condition => (Body, End).
  Builder.SetInsertPoint(Loop);
  visitStmt(F->getStep());
  Value *CondV = visitExpr(F->getCondition());
  /// End of Loop
  Builder.CreateCondBr(CondV, /* true */ Body, /* false */ End);

  /// Begin Body:
  Builder.SetInsertPoint(Body);
  if (visitStmtList(F->getBody())) {
    /// End of Body: jump back to Loop.
    Builder.CreateBr(Loop);
  }

  /// End of For.
  Builder.SetInsertPoint(End);
}

Value *LLVMIRCompiler::visitCall(Call *C) {
  Value *Callee = LocalValues[C->getFunc()];
  assert(Callee && "Callee must be created");
  std::vector<Value *> ArgsV;
  ArgsV.reserve(C->getArgs().size());
  for (Expr *A : C->getArgs()) {
    Value *Val = visitExpr(A);
    ArgsV.push_back(Val);
  }
  return Builder.CreateCall(Callee, ArgsV);
}

void LLVMIRCompiler::visitReturn(Return *Ret) {
  if (Ret->getValue()) {
    Value *Val = visitExpr(Ret->getValue());
    Builder.CreateRet(Val);
  } else {
    Builder.CreateRetVoid();
  }
}

void LLVMIRCompiler::visitAssign(Assign *A) {
  Value *RHS = visitExpr(A->getValue());
  Value *LHS = visitExpr(A->getTarget());
  Builder.CreateStore(RHS, LHS);
}

/// The logic varies depending on whether it is a load/store
Value *LLVMIRCompiler::visitSubscript(Subscript *SB) {
  Value *Array = LocalValues[SB->getName()];

  assert(Array && "Array Value must exist");
  Value *Index = visitExpr(SB->getIndex());

  /// Always remember Array values are represented by **ptr to array**
  /// and to get an address to its element, it **must** be stepped through
  /// first using a zero index in getelementptr and then the desired index.
  Value *IdxList[2] = {VM.getInt(0), Index};
  Value *ElemPtr = Builder.CreateInBoundsGEP(Array, IdxList, "elemptr");

  switch (SB->getCtx()) {
  case ExprContextKind::Load:
    /// If this is a Load, emit a load.
    return Builder.CreateLoad(ElemPtr, "elemval");
  case ExprContextKind::Store:
    /// If this is a Store, just return the ptr to the elememt
    /// to be stored by an Assign.
    return ElemPtr;
  }
}

void LLVMIRCompiler::visitRead(Read *RD) {
  /// XXX: getFunction() may have different behavior than
  /// getGlobalVariable(). Unify with GlobalValues[<name>].
  Function *Scanf = TheModule.getFunction("scanf");
  assert(Scanf && "scanf() must be declared");
  llvm::SmallVector<Value *, 2> Args;

  /// Select appropriate format specifier by type.
  auto SelectFmtSpc = [this](Expr *Name) {
    auto T = TheTable.getExprType(Name);
    switch (T) {
    case BasicTypeKind::Int:return "%d";
    case BasicTypeKind::Character:
      /// Skip one extra space.
      return " %c";
    default:llvm_unreachable("Void cannot be!");
    }
  };

  for (Expr *E : RD->getNames()) {
    Name * Nn = static_cast<Name *>(E);
    Value *FmtV = getString(SelectFmtSpc(Nn));
    Value *Var = LocalValues[Nn->getId()];
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
void LLVMIRCompiler::visitWrite(Write *WR) {
  /// A small lambda to select the appropriate format specifier.
  auto SelectFmtSpc = [WR, this]() {
    if (!WR->getValue()) {
      // No expr, no need to consult SymbolTable
      return "%s\n";
    }
    auto T = TheTable.getExprType(WR->getValue());
    if (!WR->getStr()) {
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
  if (WR->getStr())
    Args.push_back(visitExpr(WR->getStr()));
  if (WR->getValue())
    Args.push_back(visitExpr(WR->getValue()));
  Builder.CreateCall(Printf, Args);
}

/// Generate body for a Function.
void LLVMIRCompiler::visitFuncDef(FuncDef *FD) {
  /// Clear the Mapping.
  LocalValues.clear();

  /// Create function, fixing return type of main() to int.
  /// We choose to alter the AST since otherwise the AST will
  /// disagree with IR.
  if (FD->getName() == "main") {
    // XXX: Use setter: FD->setReturnType();
    FD->return_type = BasicTypeKind::Int;
  }
  /// A note about linkage:
  /// Like C, we use ExternalLinkage by default and since our
  /// program **never links** with one another, ExternalLinkage is all we
  /// have.
  Function *TheFunction = Function::Create(
      /* FunctionType */ VM.getTypeFromFuncDef(FD),
      /* Linkage */ Function::ExternalLinkage,
      /* Name */ FD->getName(),
      /* Module */ &TheModule);
  GlobalValues.emplace(FD->getName(), TheFunction);

  /// Create the entry point (Function body).
  BasicBlock *EntryBlock =
      BasicBlock::Create(TheContext, "entry", TheFunction);
  Builder.SetInsertPoint(EntryBlock);

  /// Setup arguments.
  for (llvm::Argument &Val : TheFunction->args()) {
    auto Idx = Val.getArgNo();
    ArgDecl *V = static_cast<ArgDecl *>(FD->getArgs()[Idx]);
    Val.setName(V->getName());
    /// Argument is never array.
    auto Ptr =
        Builder.CreateAlloca(VM.getType(V->getType()), nullptr, V->getName());
    /// Store the initial value of an argument.
    Builder.CreateStore(&Val, Ptr);
    LocalValues.emplace(V->getName(), Ptr);
  }

  /// Setup alloca for local storage.
  /// Setup local constants.
  for (Decl *D : FD->getDecls()) {
    if (auto VD = subclass_cast<VarDecl>(D)) {
      /// Important note about AllocaInst:
      /// The second argument ArraySize is a wrong name! It is actually
      /// NumElem. It isn't a shortcut for specifying the size of the array,
      /// but rather the number of the element of Type. %1 = alloca i32 2
      /// makes %1 a i32* -- pointer to a an i32 follow by another i32.
      /// %1 = alloca [i32 x 2]
      /// makes %1 a **pointer to a [i32 x 2] array**.
      ///
      /// For consistency with global array, we use the second notation to
      /// create local arrays. This makes the GEP as: %elemptr = getelementptr
      /// inbound [i32 x 2], [i32 x 2]* %1, i32 0, i32 <index> which is
      /// *verbose*, but consistent.
      auto Alloca = Builder.CreateAlloca(VM.getTypeFromVarDecl(VD), nullptr,
                                         VD->getName());
      LocalValues.emplace(VD->getName(), Alloca);
    } else if (auto CD = subclass_cast<ConstDecl>(D)) {
      LocalValues.emplace(CD->getName(),
                          VM.getConstantFromExpr(CD->getValue()));
    }
  }

  /// Populate LocalValues with global objects.
  SymbolTableView Local = TheTable.getLocalTable(FD);
  for (auto &&Pair : Local) {
    const SymbolEntry &E = Pair.second;
    if (E.IsLocal()) {
      assert(LocalValues.count(E.getName()) &&
          "Local Decl must have been handled");
      continue;
    }
    auto GV = GlobalValues[E.getName()];
    assert(GV && "Global Value must exist");
    LocalValues.emplace(E.getName(), GV);
  }

  /// Generate the body
  visitStmtList(FD->getStmts());

  /// Check for well-formness of all BBs. In particular, look for
  /// any unterminated BB and try to add a Return to it.
  for (BasicBlock &BB : *TheFunction) {
    Instruction *Terminator = BB.getTerminator();
    if (Terminator != nullptr)
      continue; /// Well-formed
    /// Make the function return right here.
    /// A BB is unterminated for a number of reasons:
    /// 1. the user forget to put a return here properly.
    /// 2. the function returns void, no need write return explicitly.
    /// 3. there are a branches of control flow, each of which returns
    /// properly, but there isn't a "catch-all" return. No matter what is the
    /// case, we "help" the program to return properly and
    /// **give no warning or error**.
    Builder.SetInsertPoint(&BB);
    switch (FD->getReturnType()) {
    case BasicTypeKind::Void:
      /// Make implicit return of void Function explicit.
      Builder.CreateRetVoid();
      break;
    case BasicTypeKind::Int:Builder.CreateRet(VM.getInt(0));
      break;
    case BasicTypeKind::Character:Builder.CreateRet(VM.getChar(0));
      break;
    }
  }
}

/// Create a global constant.
void LLVMIRCompiler::visitConstDecl(ConstDecl *CD) {
  // Create a global constant.
  auto GV = new llvm::GlobalVariable(
      /* Module */ TheModule,
      /* Type */ VM.getTypeFromConstDecl(CD),
      /* IsConstant */ true,
      /* Linkage */ GlobalVariable::ExternalLinkage,
      /* Initializer */ VM.getConstantFromExpr(CD->getValue()),
      /* Name */ CD->getName());
  GlobalValues.emplace(CD->getName(), GV);
}

/// Create a global variable.
void LLVMIRCompiler::visitVarDecl(VarDecl *VD) {
  auto GV = new llvm::GlobalVariable(
      /* Module */ TheModule,
      /* Type */ VM.getTypeFromVarDecl(VD),
      /* IsConstant */ false,
      /* Linkage */ GlobalVariable::ExternalLinkage,
      /* Initializer */ VM.getGlobalInitializer(VD),
      /* Name */ VD->getName());
  GlobalValues.emplace(VD->getName(), GV);
}

void LLVMIRCompiler::visitProgram(Program *P) {
  for (Decl *D : P->getDecls()) {
    visitDecl(D);
  }
  /// Verify the Module.
  String ErrorMsg;
  llvm::raw_string_ostream OS(ErrorMsg);
  if (llvm::verifyModule(TheModule, &OS)) {
    TheModule.print(llvm::errs(), nullptr);
    EM.Error(ErrorMsg);
  }
}