#ifndef SIMPLECC_TRANSFORM_EXPRESSIONTRANSFORMER_H
#define SIMPLECC_TRANSFORM_EXPRESSIONTRANSFORMER_H
#include "simplecc/Analysis/ContextualVisitor.h"

namespace simplecc {

/// This class is a CRTP base class that visits all the expressions
/// of an AST and transforms them (replace them with new Expr if Derived
/// see fit). The rationals of having such a class is:
/// 1. To change a certain node, one must call the setter of the parent node.
/// 2. Thus one must cover all the nodes that have Expr children.
/// Derived should implement a TransformExpr() method.
template <typename Derived>
class ExpressionTransformer
    : public ContextualVisitor<ExpressionTransformer<Derived>> {
  using BaseT = ContextualVisitor<ExpressionTransformer<Derived>>;

public:
  using BaseT::visitExpr;
  using BaseT::visitStmt;

  /// Default implementation.
  ExprAST *TransformExpr(ExprAST *E, AST *Parent) {
    visitExpr(E);
    return E;
  }

  /// Currently ReadStmt don't need any transformation, but
  /// for future expansion we add it here.
  void visitRead(ReadStmt *R) {}
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

public:
  ExpressionTransformer() = default;
  void Transform(Program *P, SymbolTable &S) { BaseT::visitProgram(P, S); }
};

template <typename Derived>
void ExpressionTransformer<Derived>::visitWhile(WhileStmt *W) {
  W->setCondition(
      static_cast<Derived *>(this)->TransformExpr(W->getCondition(), W));
  for (StmtAST *S : W->getBody())
    visitStmt(S);
}

template <typename Derived>
void ExpressionTransformer<Derived>::visitWrite(WriteStmt *W) {
  if (!W->getValue())
    return;
  W->setValue(static_cast<Derived *>(this)->TransformExpr(W->getValue(), W));
}

template <typename Derived>
void ExpressionTransformer<Derived>::visitAssign(AssignStmt *A) {
  A->setValue(static_cast<Derived *>(this)->TransformExpr(A->getValue(), A));
  A->setTarget(static_cast<Derived *>(this)->TransformExpr(A->getTarget(), A));
}

template <typename Derived>
void ExpressionTransformer<Derived>::visitFor(ForStmt *F) {
  visitStmt(F->getInitial());
  F->setCondition(
      static_cast<Derived *>(this)->TransformExpr(F->getCondition(), F));
  visitStmt(F->getStep());
  for (StmtAST *S : F->getBody())
    visitStmt(S);
}

template <typename Derived>
void ExpressionTransformer<Derived>::visitReturn(ReturnStmt *R) {
  if (!R->getValue())
    return;
  R->setValue(static_cast<Derived *>(this)->TransformExpr(R->getValue(), R));
}

template <typename Derived>
void ExpressionTransformer<Derived>::visitIf(IfStmt *I) {
  I->setTest(static_cast<Derived *>(this)->TransformExpr(I->getTest(), I));
  for (auto S : I->getBody())
    visitStmt(S);
  for (auto S : I->getOrelse())
    visitStmt(S);
}

template <typename Derived>
void ExpressionTransformer<Derived>::visitCall(CallExpr *C) {
  for (unsigned I = 0, E = C->getNumArgs(); I < E; I++) {
    C->setArgAt(I,
                static_cast<Derived *>(this)->TransformExpr(C->getArgAt(I), C));
  }
}

template <typename Derived>
void ExpressionTransformer<Derived>::visitBinOp(BinOpExpr *B) {
  B->setLeft(static_cast<Derived *>(this)->TransformExpr(B->getLeft(), B));
  B->setRight(static_cast<Derived *>(this)->TransformExpr(B->getRight(), B));
}

template <typename Derived>
void ExpressionTransformer<Derived>::visitBoolOp(BoolOpExpr *B) {
  B->setValue(static_cast<Derived *>(this)->TransformExpr(B->getValue(), B));
}

template <typename Derived>
void ExpressionTransformer<Derived>::visitParenExpr(ParenExpr *PE) {
  PE->setValue(static_cast<Derived *>(this)->TransformExpr(PE->getValue(), PE));
}

template <typename Derived>
void ExpressionTransformer<Derived>::visitUnaryOp(UnaryOpExpr *U) {
  U->setOperand(
      static_cast<Derived *>(this)->TransformExpr(U->getOperand(), U));
}

template <typename Derived>
void ExpressionTransformer<Derived>::visitSubscript(SubscriptExpr *SB) {
  SB->setIndex(static_cast<Derived *>(this)->TransformExpr(SB->getIndex(), SB));
}
} // namespace simplecc

#endif // SIMPLECC_TRANSFORM_EXPRESSIONTRANSFORMER_H