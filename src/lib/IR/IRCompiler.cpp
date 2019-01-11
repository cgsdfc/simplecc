#include "simplecc/IR/IRCompiler.h"
#include "simplecc/IR/Function.h"
#include "simplecc/IR/Module.h"

using namespace simplecc;

void IRCompiler::visitFuncDef(FuncDef *FD) {
  ContextualVisitor::setLocalTable(FD);
  LocalValueMap.clear();
  auto TheFunction = Function::Create(
      /* ReturnType */ get(FD->getReturnType()),
      /* NumArgs */ FD->getNumArgs(),
      /* Name */ FD->getName(),
      /* Module */ TheModule);
  GlobalValueMap.emplace(FD->getName(), TheFunction);
  BasicBlock *EntryBlock = BasicBlock::Create(TheFunction);
  Builder.SetInsertPoint(EntryBlock);
  for (Argument *Val : TheFunction->args()) {
    visitArgDecl(FD->getArgAt(Val->getArgNo()), Val);
  }
  for (DeclAST *Decl : FD->getDecls()) {
    if (!IsInstance<VarDecl>(Decl))
      continue;
    // ConstDecl should have been folded.
    visitVarDecl(static_cast<VarDecl *>(Decl), /* isLocal */ true);
  }
  visitStmtList(FD->getStmts());
  EnsureProperReturn(TheFunction);
}

Type *IRCompiler::get(BasicTypeKind Ty) const {
  switch (Ty) {
  case BasicTypeKind::Int:
  case BasicTypeKind::Character: return Builder.getIntType();
  case BasicTypeKind::Void: return Builder.getVoidType();
  }
}

void IRCompiler::visitArgDecl(ArgDecl *AD, Argument *Val) {
  auto Ptr = Builder.CreateAlloca(1);
  Builder.CreateStore(Val, Ptr);
  LocalValueMap.emplace(AD->getName(), Ptr);
}

void IRCompiler::visitVarDecl(VarDecl *VD, bool isLocal) {
  if (isLocal) {
    auto Ptr = Builder.CreateAlloca(getSize(VD));
    LocalValueMap.emplace(VD->getName(), Ptr);
    return;
  }
  auto GV = GlobalVariable::Create(getSize(VD), TheModule);
  GlobalValueMap.emplace(VD->getName(), GV);
}

unsigned int IRCompiler::getSize(VarDecl *VD) {
  return VD->isArray() ? VD->getSize() : 1;
}

Value *IRCompiler::get(SymbolEntry Entry) {
  auto Val = Entry.IsGlobal() ?
             GlobalValueMap[Entry.getName()] : LocalValueMap[Entry.getName()];
  assert(Val && "Absent Value");
  return Val;
}

void IRCompiler::EnsureProperReturn(Function *F) {
  for (BasicBlock *BB : *F) {
    Instruction *Terminator = BB->getTerminator();
    if (Terminator != nullptr)
      continue;
    Builder.SetInsertPoint(BB);
    switch (F->getReturnType()->getTypeID()) {
    case Type::IntTypeKind:Builder.CreateRet(Builder.getInt(0));
      break;
    case Type::VoidTypeKind:Builder.CreateRetVoid();
      break;
    default:assert(false && "Impossible Return Type");
    }
  }
}

void IRCompiler::visitProgram(ProgramAST *P) {
  for (auto Decl : P->getDecls()) {
    switch (Decl->getKind()) {
    case DeclAST::FuncDefKind:visitFuncDef(static_cast<FuncDef *>(Decl));
      break;
    case DeclAST::VarDeclKind:visitVarDecl(static_cast<VarDecl *>(Decl), /* isLocal */ false);
      break;
    default:break;
    }
  }
}

void IRCompiler::visitWrite(WriteStmt *WR) {
  ChildrenVisitor::visitWrite(WR);
}

void IRCompiler::visitRead(ReadStmt *RD) {
  ChildrenVisitor::visitRead(RD);
}

void IRCompiler::visitAssign(AssignStmt *A) {
  auto Val = visitExpr(A->getValue());
  auto Ptr = visitExpr(A->getTarget());
  Builder.CreateStore(Val, Ptr);
}

void IRCompiler::visitReturn(ReturnStmt *R) {
  R->getValue() ? Builder.CreateRet(visitExpr(R->getValue()))
                : Builder.CreateRetVoid();
}

void IRCompiler::visitFor(ForStmt *F) {
  Function *TheFunction = Builder.GetInsertBlock()->getParent();
  auto Body = BasicBlock::Create(TheFunction);
  auto Loop = BasicBlock::Create(TheFunction);
  auto End = BasicBlock::Create(TheFunction);

  visitStmt(F->getInitial());
  Builder.CreateBr(Body);

  Builder.SetInsertPoint(Loop);
  visitStmt(F->getStep());
  auto Cond = visitExpr(F->getCondition());
  Builder.CreateCondBr(Cond, Body, End);

  Builder.SetInsertPoint(Body);
  visitStmtList(F->getBody());
  if (!Body->getTerminator()) {
    Builder.CreateBr(Loop);
  }

  Builder.SetInsertPoint(End);
}

void IRCompiler::visitWhile(WhileStmt *W) {
  Function *TheFunction = Builder.GetInsertBlock()->getParent();
  auto Loop = BasicBlock::Create(TheFunction);
  Builder.CreateBr(Loop);

  Builder.SetInsertPoint(Loop);
  auto Cond = visitExpr(W->getCondition());
  auto Body = BasicBlock::Create(TheFunction);
  auto End = BasicBlock::Create(TheFunction);
  Builder.CreateCondBr(Cond, Body, End);

  Builder.SetInsertPoint(Body);
  visitStmtList(W->getBody());
  if (!Body->getTerminator()) {
    Builder.CreateBr(Loop);
  }

  Builder.SetInsertPoint(End);
}

void IRCompiler::visitIf(IfStmt *I) {
  Function *TheFunction = Builder.GetInsertBlock()->getParent();
  auto Cond = visitExpr(I->getTest());
  auto Then = BasicBlock::Create(TheFunction);
  auto Else = BasicBlock::Create(TheFunction);
  Builder.CreateCondBr(Cond, Then, Else);

  Builder.SetInsertPoint(Then);
  visitStmtList(I->getBody());
  Builder.SetInsertPoint(Else);
  visitStmtList(I->getOrelse());
  if (Then->getTerminator() && Else->getTerminator()) {
    // Both Then and Else terminates.
    return;
  }
  auto End = BasicBlock::Create(TheFunction);
  if (!Then->getTerminator()) {
    BranchInst::Create(End, Then);
  }
  if (!Else->getTerminator()) {
    BranchInst::Create(End, Else);
  }

  Builder.SetInsertPoint(End);
}

void IRCompiler::visitStmtList(const StmtAST::StmtListType &StmtList) {
  for (auto Stmt : StmtList) {
    visitStmt(Stmt);
  }
}

// TODO: make it shorter.
Value *IRCompiler::visitBinOp(BinOpExpr *B) {
  auto L = visitExpr(B->getLeft());
  auto R = visitExpr(B->getRight());
  switch (B->getOp()) {
  case OperatorKind::Add:return Builder.CreateAdd(L, R);
  case OperatorKind::Sub:return Builder.CreateSub(L, R);
  case OperatorKind::Mult:return Builder.CreateMul(L, R);
  case OperatorKind::Div:return Builder.CreateDiv(L, R);
  case OperatorKind::Eq:return Builder.CreateICmpEQ(L, R);
  case OperatorKind::NotEq:return Builder.CreateICmpNE(L, R);
  case OperatorKind::Lt:return Builder.CreateICmpSLT(L, R);
  case OperatorKind::LtE:return Builder.CreateICmpSLE(L, R);
  case OperatorKind::Gt:return Builder.CreateICmpSGT(L, R);
  case OperatorKind::GtE:return Builder.CreateICmpSGE(L, R);
  }
}

Value *IRCompiler::visitUnaryOp(UnaryOpExpr *U) {
  Value *Operand = visitExpr(U->getOperand());
  switch (U->getOp()) {
  case UnaryopKind::USub:return Builder.CreateNeg(Operand);
  case UnaryopKind::UAdd:return Operand;
  }
}

Value *IRCompiler::visitSubscript(SubscriptExpr *S) {
  Value *Array = get(S->getArrayName());
  Value *Index = visitExpr(S->getIndex());
  Value *GEP = Builder.CreateGEP(Array, Index);
  return maybeLoad(GEP, S->getContext());
}

Value *IRCompiler::visitCall(CallExpr *C) {
  Value *Callee = get(C->getCallee());
  std::vector<Value *> Args(C->getNumArgs());
  for (unsigned I = 0, E = C->getNumArgs(); I != E; ++I) {
    Args[I] = visitExpr(C->getArgAt(I));
  }
  return Builder.CreateCall(Callee, Args);
}

Value *IRCompiler::visitBoolOp(BoolOpExpr *B) {
  auto Val = visitExpr(B->getValue());
  if (B->hasCompareOp())
    return Val;
  return Builder.CreateICmpEQ(Val, Builder.getTrue());
}

Value *IRCompiler::visitParenExpr(ParenExpr *P) {
  return visitExpr(P->getValue());
}

Value *IRCompiler::visitNum(NumExpr *N) {
  return Builder.getInt(N->getNum());
}

Value *IRCompiler::visitChar(CharExpr *C) {
  return Builder.getInt(C->getChar());
}

Value *IRCompiler::visitName(NameExpr *N) {
  auto Ptr = get(N->getId());
  return maybeLoad(Ptr, N->getContext());
}

Value *IRCompiler::maybeLoad(Value *Ptr, ExprContextKind ExprContext) {
  assert(Ptr->getType()->isPointerType());
  switch (ExprContext) {
  case ExprContextKind::Load: return Builder.CreateLoad(Ptr);
  case ExprContextKind::Store: return Ptr;
  }
}

Value *IRCompiler::visitStr(StrExpr *S) {
  return Builder.getStringLiteral(S->getStr());
}

bool IRCompiler::Compile(ProgramAST *P, const SymbolTable &S, Module &M) {
  ContextualVisitor::setTable(const_cast<SymbolTable &>(S));
  TheModule = &M;
  visitProgram(P);
  return false;
}