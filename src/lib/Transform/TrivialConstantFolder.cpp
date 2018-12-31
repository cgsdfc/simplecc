#include <simplecc/Transform/TrivialConstantFolder.h>
#include <functional>

using namespace simplecc;

// TODO: don't use such a template.
template<typename BinaryFunc>
static inline int Compute(BinaryFunc Op, int L, int R) {
  return Op(L, R);
}

Expr *TrivialConstantFolder::visitBinOp(BinOpExpr *B) {
  Expr *L = B->getLeft();
  Expr *R = B->getRight();

  // Case-1: both side is non-constant.
  if (!L->isConstant() && !R->isConstant()) {
    // No opportunity.
    return B;
  }
  // Case-2: both side is constant, evaluate it directly.
  if (L->isConstant() && R->isConstant()) {
    // If we see a zero divisor, left it to runtime.
    if (B->getOp() == OperatorKind::Div && R->isZeroVal()) {
      // ZeroDivisorError.
      return B;
    }
    switch (B->getOp()) {
    default:assert(false && "Unhandled Enum Value");
#define HANDLE_OPERATOR(VAL, OP, FUNC)                                         \
  case OperatorKind::VAL:                                                      \
    return new NumExpr(Compute(std::FUNC<int>(), L->getConstantValue(),        \
                               R->getConstantValue()),                         \
                       B->getLocation());
#include "simplecc/Parse/Enums.def"
    }
  }

  // Case-3: single side constant.
  switch (B->getOp()) {
  case OperatorKind::Add:
    // 0 + X == X + 0 == X
    if (L->isZeroVal())
      return static_cast<Expr *>(std::move(*B).getRight().release());
    if (R->isZeroVal())
      return static_cast<Expr *>(std::move(*B).getLeft().release());
    return B;
  case OperatorKind::Sub:
    // 0 - X == -X
    if (L->isZeroVal())
      return new UnaryOpExpr(
          UnaryopKind::USub,
          static_cast<Expr *>(std::move(*B).getRight().release()),
          B->getLocation());
    return B;
  case OperatorKind::Mult:
    // 0 * X == X * 0 == 0
    if (L->isZeroVal() || R->isZeroVal())
      return new NumExpr(0, B->getLocation());
    // 1 * X == X * 1 == X
    if (L->isOneVal())
      return static_cast<Expr *>(std::move(*B).getRight().release());
    if (R->isOneVal())
      return static_cast<Expr *>(std::move(*B).getLeft().release());
    return B;
  case OperatorKind::Div:
    // 0 / X == 0
    if (L->isZeroVal())
      return new NumExpr(0, B->getLocation());
    // X / 1 == X
    if (R->isOneVal())
      return static_cast<Expr *>(std::move(*B).getLeft().release());
    return B;
  default:return B;
  }
}

Expr *TrivialConstantFolder::visitUnaryOp(UnaryOpExpr *U) {
  // Case-1: Ignore UAdd, +X => X
  if (U->getOp() == UnaryopKind::UAdd) {
    return static_cast<Expr *>(std::move(*U).getOperand().release());
  }
  // Case-2: Compute negate of constant like -1, -2.
  assert(U->getOp() == UnaryopKind::USub);
  auto Operand = U->getOperand();
  if (Operand->isConstant()) {
    return new NumExpr(-Operand->getConstantValue(), U->getLocation());
  }

  // Case-3: fold double negate into noop, --X => X
  if (IsInstance<UnaryOpExpr>(Operand) &&
      static_cast<UnaryOpExpr *>(Operand)->getOp() == UnaryopKind::USub) {
    auto UO = static_cast<UnaryOpExpr *>(Operand);
    return static_cast<Expr *>(std::move(*UO).getOperand().release());
  }
  return U;
}

Expr *TrivialConstantFolder::visitParenExpr(ParenExpr *P) {
  // Extract wrapped value, (X) => X
  return static_cast<Expr *>(std::move(*P).getValue().release());
}

Expr *TrivialConstantFolder::visitExpr(Expr *E) {
  switch (E->getKind()) {
  case Expr::BinOpExprKind:return visitBinOp(static_cast<BinOpExpr *>(E));
  case Expr::ParenExprKind:return visitParenExpr(static_cast<ParenExpr *>(E));
  case Expr::UnaryOpExprKind:return visitUnaryOp(static_cast<UnaryOpExpr *>(E));
  default:return E;
  }
}

Expr *TrivialConstantFolder::TransformExpr(Expr *E, AST *Parent) {
  ExpressionTransformer::visitExpr(E);
  return visitExpr(E);
}