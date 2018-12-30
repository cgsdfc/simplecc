#ifndef SIMPLECC_TRANSFORM_EXPRTRANSFORMER_H
#define SIMPLECC_TRANSFORM_EXPRTRANSFORMER_H
#include "simplecc/Analysis/ContextualVisitor.h"

namespace simplecc {

/// This class is a CRTP base class that visits all the expressions
/// of an AST and transforms them (replace them with new Expr if Derived
/// see fit). The rationals of having such a class is:
/// 1. To change a certain node, one must call the setter of the parent node.
/// 2. Thus one must cover all the nodes that have Expr children.
/// Derived should implement a TransformExpr() method.
template<typename Derived>
class ExprTransformer : public ContextualVisitor<ExprTransformer<Derived>> {
  using BaseT = ContextualVisitor<ExprTransformer<Derived>>;
public:
  using BaseT::visitStmt;
  using BaseT::visitExpr;

  /// Default implementation.
  Expr *TransformExpr(Expr *E, AST *Parent) {
    visitExpr(E);
    return E;
  }

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
  ExprTransformer() = default;
  void Transform(Program *P, SymbolTable &S) {
    ContextualVisitor<Derived>::visitProgram(P, S);
  }
};

template<typename Derived>
void ExprTransformer<Derived>::visitWhile(WhileStmt *W) {
  W->setCondition(
      static_cast<Derived *>(this)->TransformExpr(W->getCondition(), W)
  );
  for (Stmt *S : W->getBody())
    visitStmt(S);
}

template<typename Derived>
void ExprTransformer<Derived>::visitWrite(WriteStmt *W) {
  W->setValue(
      static_cast<Derived *>(this)->TransformExpr(W->getValue(), W)
  );
}

template<typename Derived>
void ExprTransformer<Derived>::visitAssign(AssignStmt *A) {
  A->setValue(
      static_cast<Derived *>(this)->TransformExpr(A->getValue(), A)
  );
}

template<typename Derived>
void ExprTransformer<Derived>::visitFor(ForStmt *F) {
  visitStmt(F->getInitial());
  F->setCondition(
      static_cast<Derived *>(this)->TransformExpr(F->getCondition(), F)
  );
  visitStmt(F->getStep());
  for (Stmt *S : F->getBody())
    visitStmt(S);
}

template<typename Derived>
void ExprTransformer<Derived>::visitReturn(ReturnStmt *R) {
  R->setValue(
      static_cast<Derived *>(this)->TransformExpr(R->getValue(), R)
  );
}

template<typename Derived>
void ExprTransformer<Derived>::visitIf(IfStmt *I) {
  I->setTest(
      static_cast<Derived *>(this)->TransformExpr(I->getTest(), I)
  );
  for (auto S : I->getBody())
    visitStmt(S);
  for (auto S : I->getOrelse())
    visitStmt(S);
}

template<typename Derived>
void ExprTransformer<Derived>::visitCall(CallExpr *C) {
  for (unsigned I = 0, E = C->getNumArgs(); I < E; I++) {
    C->setArgAt(
        I, static_cast<Derived *>(this)->TransformExpr(C->getArgAt(I), C)
    );
  }
}

template<typename Derived>
void ExprTransformer<Derived>::visitBinOp(BinOpExpr *B) {
  B->setLeft(
      static_cast<Derived *>(this)->TransformExpr(B->getLeft(), B)
  );
  B->setRight(
      static_cast<Derived *>(this)->TransformExpr(B->getRight(), B)
  );
}

template<typename Derived>
void ExprTransformer<Derived>::visitBoolOp(BoolOpExpr *B) {
  B->setValue(
      static_cast<Derived *>(this)->TransformExpr(B->getValue(), B)
  );
}

template<typename Derived>
void ExprTransformer<Derived>::visitParenExpr(ParenExpr *PE) {
  PE->setValue(
      static_cast<Derived *>(this)->TransformExpr(PE->getValue(), PE)
  );
}

template<typename Derived>
void ExprTransformer<Derived>::visitUnaryOp(UnaryOpExpr *U) {
  U->setOperand(
      static_cast<Derived *>(this)->TransformExpr(U->getOperand(), U)
  );
}

template<typename Derived>
void ExprTransformer<Derived>::visitSubscript(SubscriptExpr *SB) {
  SB->setIndex(
      static_cast<Derived *>(this)->TransformExpr(SB->getIndex(), SB)
  );
}
}

#endif //SIMPLECC_TRANSFORM_EXPRTRANSFORMER_H
