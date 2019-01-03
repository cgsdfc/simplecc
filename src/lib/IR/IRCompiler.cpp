#include "simplecc/IR/IRCompiler.h"
#include "simplecc/IR/Function.h"
#include "simplecc/IR/Argument.h"
#include "simplecc/IR/Module.h"
#include "simplecc/IR/GlobalVariable.h"

using namespace simplecc;

void IRCompiler::visitFuncDef(FuncDef *FD) {
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
    // ConstDecl should be folded.
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

void IRCompiler::visitProgram(Program *P) {
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
  ChildrenVisitor::visitFor(F);
}

void IRCompiler::visitWhile(WhileStmt *W) {
  ChildrenVisitor::visitWhile(W);
}

void IRCompiler::visitIf(IfStmt *I) {
  ChildrenVisitor::visitIf(I);
}

bool IRCompiler::visitStmtList(const StmtAST::StmtListType &StmtList) {

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
  Value *Array = get(S->getName());
  Value *Index = visitExpr(S->getIndex());
  Value *GEP = Builder.CreateGEP(Array, Index);
  return maybeLoad(GEP, S->getContext());
}

Value *IRCompiler::visitCall(CallExpr *C) {
  Value *Callee = get(C->getFunc());
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
  return Builder.getInt(N->getN());
}

Value *IRCompiler::visitChar(CharExpr *C) {
  return Builder.getInt(C->getC());
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
  return Builder.getStringLiteral(S->getS());
}

bool IRCompiler::Compile(Program *P, const SymbolTable &S, Module &M) {
  return false;
}
