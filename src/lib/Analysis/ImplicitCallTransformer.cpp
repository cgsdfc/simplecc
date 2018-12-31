// XXX: Since this pass **modifies** the AST, it is important
// to run a verify pass on it to ensure it didn't break any invariant.
#include "simplecc/Analysis/ImplicitCallTransformer.h"

using namespace simplecc;

void ImplicitCallTransformer::visitWrite(WriteStmt *W) {
  W->setValue(TransformExpr(W->getValue()));
}

void ImplicitCallTransformer::visitAssign(AssignStmt *A) {
  /// Don't transform the target!
  visitExpr(A->getTarget());
  A->setValue(TransformExpr(A->getValue()));
}

void ImplicitCallTransformer::visitFor(ForStmt *F) {
  visitStmt(F->getInitial());
  F->setCondition(TransformExpr(F->getCondition()));
  visitStmt(F->getStep());
  for (auto s : F->getBody()) {
    visitStmt(s);
  }
}

void ImplicitCallTransformer::visitWhile(WhileStmt *W) {
  W->setCondition(TransformExpr(W->getCondition()));
  for (auto s : W->getBody()) {
    visitStmt(s);
  }
}

void ImplicitCallTransformer::visitReturn(ReturnStmt *R) {
  R->setValue(TransformExpr(R->getValue()));
}

void ImplicitCallTransformer::visitIf(IfStmt *I) {
  I->setTest(TransformExpr(I->getTest()));
  for (auto s : I->getBody()) {
    visitStmt(s);
  }
  for (auto s : I->getOrelse()) {
    visitStmt(s);
  }
}

void ImplicitCallTransformer::visitBinOp(BinOpExpr *B) {
  B->setLeft(TransformExpr(B->getLeft()));
  B->setRight(TransformExpr(B->getRight()));
}

void ImplicitCallTransformer::visitCall(CallExpr *C) {
  for (unsigned I = 0, E = C->getNumArgs(); I < E; I++) {
    C->setArgAt(I, TransformExpr(C->getArgAt(I)));
  }
}

void ImplicitCallTransformer::visitFuncDef(FuncDef *FD) {
  setLocalTable(TheTable->getLocalTable(FD));
  ChildrenVisitor::visitFuncDef(FD);
}

/// Visit an Expr and do implicit call transform on it if applicable.
/// Note: This is deliberately not named after visitExpr() to prevent
/// ChildrenVisitor accidentally call it, which may lead to unexpected
/// result. For example, we don't override visitRead() since it **does not**
/// need transformation. But ChildrenVisitor will still call it and visit all
/// its children with visitExpr() so it is important that visitExpr() don't
/// perform any transformation.
ExprAST *ImplicitCallTransformer::TransformExpr(ExprAST *E) {
  if (!E)
    return E;
  if (!IsInstance<NameExpr>(E)) {
    visitExpr(E);
    return E;
  }
  NameExpr *N = static_cast<NameExpr *>(E);
  if (!TheLocalTable[N->getId()].IsFunction()) {
    return E;
  }
  return new CallExpr(N->getId(), {}, E->getLocation());
}

/// Perform implicit call transform on the program using a SymbolTable.
void ImplicitCallTransformer::Transform(Program *P, const SymbolTable &S) {
  assert(P);
  setTable(&S);
  visitProgram(P);
}

void ImplicitCallTransformer::visitBoolOp(BoolOpExpr *B) {
  B->setValue(TransformExpr(B->getValue()));
}
void ImplicitCallTransformer::visitParenExpr(ParenExpr *PE) {
  PE->setValue(TransformExpr(PE->getValue()));
}
void ImplicitCallTransformer::visitUnaryOp(UnaryOpExpr *U) {
  U->setOperand(TransformExpr(U->getOperand()));
}
void ImplicitCallTransformer::visitSubscript(SubscriptExpr *SB) {
  SB->setIndex(TransformExpr(SB->getIndex()));
}