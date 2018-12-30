#ifndef SIMPLECC_TRANSFORMER_H
#define SIMPLECC_TRANSFORMER_H
#include "simplecc/Analysis/Visitor.h"

namespace simplecc {

/// This class is a CRTP base class that visits all the expressions
/// of an AST and transforms them (replace them with new Expr if Derived
/// see fit). The rationals of having such a class is:
/// 1. To change a certain node, one must call the setter of the parent node.
/// 2. Thus one must cover all the nodes that have Expr children.
/// Derived should implement a TransformExpr() method.
template<typename Derived>
class ExprTransformer : public ChildrenVisitor<ExprTransformer<Derived>> {
public:
  using ChildrenVisitor<Derived>::visitStmt;
  using ChildrenVisitor<Derived>::visitExpr;

  /// Default implementation.
  Expr *TransformExpr(Expr *E, AST *Parent) { return E; }

  void visitWrite(WriteStmt *W);
  void visitAssign(AssignStmt *A);
  void visitFor(ForStmt *F);
  void visitWhile(WhileStmt *W);
  void visitReturn(ReturnStmt *R);
  void visitIf(IfStmt *I);

  void visitCall(CallExpr *C);
  void visitBinOp(BinOpExpr *B);
  void visitBoolOp(BoolOpExpr *B);
  void visitParenExpr(ParenExpr *PE);
  void visitUnaryOp(UnaryOpExpr *U);
  void visitSubscript(SubscriptExpr *SB);

  void visitFuncDef(FuncDef *FD);
};

template<typename Derived>
void ExprTransformer<Derived>::visitWhile(WhileStmt *W) {
  W->setCondition(
      static_cast<Derived *>(this)->TransformExpr(W->getCondition(), W));
  for (Stmt *S : W->getBody())
    visitStmt(S);
}

template<typename Derived>
void ExprTransformer<Derived>::visitWrite(WriteStmt *W) {
  W->setValue(
      static_cast<Derived *>(this)->TransformExpr(W->getValue(), W));
}

template<typename Derived>
void ExprTransformer<Derived>::visitAssign(AssignStmt *A) {
  A->setValue(static_cast<Derived *>(this)->TransformExpr(A->getValue()), A);
}

template<typename Derived>
void ExprTransformer<Derived>::visitFor(ForStmt *F) {
  visitStmt(F->getInitial());
  F->setCondition(static_cast<Derived *>(this)->TransformExpr(F->getCondition()));
  visitStmt(F->getStep());
  for (Stmt *S : F->getBody())
    visitStmt(S);
}

template<typename Derived>
void ExprTransformer<Derived>::visitReturn(ReturnStmt *R) {
  R->setValue(static_cast<Derived *>(this)->TransformExpr(R->getValue()));
}

template<typename Derived>
void ExprTransformer<Derived>::visitIf(IfStmt *I) {
  I->setTest(
      static_cast<Derived *>(this)->TransformExpr(I->getTest()));
  for (auto S : I->getBody())
    visitStmt(S);
  for (auto S : I->getOrelse())
    visitStmt(S);
}

template<typename Derived>
void ExprTransformer<Derived>::visitCall(CallExpr *C) {
  for (unsigned I = 0, E = C->getNumArgs(); I < E; I++) {
    C->setArgAt(I, static_cast<Derived *>(this)->TransformExpr(C->getArgAt(I)));
  }
}

template<typename Derived>
void ExprTransformer<Derived>::visitBinOp(BinOpExpr *B) {
  B->setLeft(
      static_cast<Derived *>(this)->TransformExpr(B->getLeft()));
  B->setRight(
      static_cast<Derived *>(this)->TransformExpr(B->getRight()));
}

template<typename Derived>
void ExprTransformer<Derived>::visitBoolOp(BoolOpExpr *B) {
  B->setValue(
      static_cast<Derived *>(this)->TransformExpr(B->getValue()));
}

template<typename Derived>
void ExprTransformer<Derived>::visitParenExpr(ParenExpr *PE) {
  PE->setValue(
      static_cast<Derived *>(this)->TransformExpr(PE->getValue()));
}

template<typename Derived>
void ExprTransformer<Derived>::visitUnaryOp(UnaryOpExpr *U) {
  U->setOperand(
      static_cast<Derived *>(this)->TransformExpr(U->getOperand()));
}

template<typename Derived>
void ExprTransformer<Derived>::visitSubscript(SubscriptExpr *SB) {
  SB->setIndex(
      static_cast<Derived *>(this)->TransformExpr(SB->getIndex()));
}

}

#endif //SIMPLECC_TRANSFORMER_H
