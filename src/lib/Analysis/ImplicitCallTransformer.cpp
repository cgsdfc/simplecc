// XXX: Since this pass **modifies** the AST, it is important
// to run a verify pass on it to ensure it didn't break any invariant.
#include "simplecc/Analysis/ImplicitCallTransformer.h"

using namespace simplecc;

void ImplicitCallTransformer::visitWrite(WriteStmt *W) {
  if (W->getValue()) {
    TransformExpr(W->value);
  }
}

void ImplicitCallTransformer::visitAssign(AssignStmt *A) {
  /// Don't transform the target!
  visitExpr(A->getTarget());
  TransformExpr(A->value);
}

void ImplicitCallTransformer::visitFor(ForStmt *F) {
  visitStmt(F->initial);
  TransformExpr(F->condition);
  visitStmt(F->getStep());
  for (auto s : F->getBody()) {
    visitStmt(s);
  }
}

void ImplicitCallTransformer::visitWhile(WhileStmt *W) {
  TransformExpr(W->condition);
  for (auto s : W->getBody()) {
    visitStmt(s);
  }
}

void ImplicitCallTransformer::visitReturn(ReturnStmt *R) {
  if (R->getValue()) {
    TransformExpr(R->value);
  }
}

void ImplicitCallTransformer::visitIf(IfStmt *I) {
  TransformExpr(I->test);
  for (auto s : I->getBody()) {
    visitStmt(s);
  }
  for (auto s : I->getOrelse()) {
    visitStmt(s);
  }
}

void ImplicitCallTransformer::visitBinOp(BinOpExpr *B) {
  TransformExpr(B->left);
  TransformExpr(B->right);
}

void ImplicitCallTransformer::visitCall(CallExpr *C) {
  for (int I = 0, Size = C->getArgs().size(); I < Size; I++) {
    TransformExpr(C->args[I]);
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
/// Note: This modifies its argument with reference!
void ImplicitCallTransformer::TransformExpr(Expr *&E) {
  if (!IsInstance<NameExpr>(E))
    return visitExpr(E);
  NameExpr *N = static_cast<NameExpr *>(E);
  if (!TheLocalTable[N->getId()].IsFunction())
    return visitExpr(E);
  CallExpr *C = new CallExpr(N->getId(), {}, E->getLocation());
  delete E;
  E = C;
}

/// Perform implicit call transform on the program using a SymbolTable.
void ImplicitCallTransformer::Transform(Program *P, const SymbolTable &S) {
  assert(P);
  setTable(&S);
  visitProgram(P);
}