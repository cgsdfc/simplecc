// XXX: Since this pass **modifies** the AST, it is important
// to run a verify pass on it to ensure it didn't break any invariant.
#include "simplecc/ImplicitCallTransformer.h"
#include <cassert>

using namespace simplecc;

void ImplicitCallTransformer::visitWrite(Write *W) {
  if (W->getValue()) {
    TransformExpr(W->value);
  }
}

void ImplicitCallTransformer::visitAssign(Assign *A) {
  /// Don't transform the target!
  visitExpr(A->getTarget());
  TransformExpr(A->value);
}

void ImplicitCallTransformer::visitFor(For *F) {
  visitStmt(F->initial);
  TransformExpr(F->condition);
  visitStmt(F->getStep());
  for (auto s : F->getBody()) {
    visitStmt(s);
  }
}

void ImplicitCallTransformer::visitWhile(While *W) {
  TransformExpr(W->condition);
  for (auto s : W->getBody()) {
    visitStmt(s);
  }
}

void ImplicitCallTransformer::visitReturn(Return *R) {
  if (R->getValue()) {
    TransformExpr(R->value);
  }
}

void ImplicitCallTransformer::visitIf(If *I) {
  TransformExpr(I->test);
  for (auto s : I->getBody()) {
    visitStmt(s);
  }
  for (auto s : I->getOrelse()) {
    visitStmt(s);
  }
}

void ImplicitCallTransformer::visitBinOp(BinOp *B) {
  TransformExpr(B->left);
  TransformExpr(B->right);
}

void ImplicitCallTransformer::visitCall(Call *C) {
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
  if (!IsInstance<Name>(E))
    return visitExpr(E);
  Name *N = static_cast<Name *>(E);
  if (!TheLocalTable[N->getId()].IsFunction())
    return visitExpr(E);
  Call *C = new Call(N->getId(), {}, E->getLoc());
  delete E;
  E = C;
}

/// Perform implicit call transform on the program using a SymbolTable.
void ImplicitCallTransformer::Transform(Program *P, const SymbolTable &S) {
  assert(P);
  setTable(&S);
  visitProgram(P);
}

namespace simplecc {
void TransformImplicitCall(Program *P, const SymbolTable &S) {
  ImplicitCallTransformer().Transform(P, S);
}
} // namespace simplecc