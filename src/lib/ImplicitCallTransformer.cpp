// XXX: Since this pass **modifies** the AST, it is important
// to run a verify pass on it to ensure it didn't break any invariant.
#include "simplecc/ImplicitCallTransformer.h"
#include <cassert>

using namespace simplecc;

void ImplicitCallTransformer::visitWrite(Write *node) {
  if (node->getValue()) {
    TransformExpr(node->value);
  }
}

void ImplicitCallTransformer::visitAssign(Assign *node) {
  /// Don't transform the target!
  visitExpr(node->getTarget());
  TransformExpr(node->value);
}

void ImplicitCallTransformer::visitFor(For *node) {
  visitStmt(node->initial);
  TransformExpr(node->condition);
  visitStmt(node->getStep());
  for (auto s : node->getBody()) {
    visitStmt(s);
  }
}

void ImplicitCallTransformer::visitWhile(While *node) {
  TransformExpr(node->condition);
  for (auto s : node->getBody()) {
    visitStmt(s);
  }
}

void ImplicitCallTransformer::visitReturn(Return *node) {
  if (node->getValue()) {
    TransformExpr(node->value);
  }
}

void ImplicitCallTransformer::visitIf(If *node) {
  TransformExpr(node->test);
  for (auto s : node->getBody()) {
    visitStmt(s);
  }
  for (auto s : node->getOrelse()) {
    visitStmt(s);
  }
}

void ImplicitCallTransformer::visitBinOp(BinOp *node) {
  TransformExpr(node->left);
  TransformExpr(node->right);
}

void ImplicitCallTransformer::visitCall(Call *node) {
  for (int I = 0, Size = node->getArgs().size(); I < Size; I++) {
    TransformExpr(node->args[I]);
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
  if (Name *N = static_cast<Name *>(E);
      !TheLocalTable[N->getId()].IsFunction())
    return visitExpr(E);
  Call *C = new Call(static_cast<Name *>(E)->getId(), {}, E->getLoc());
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