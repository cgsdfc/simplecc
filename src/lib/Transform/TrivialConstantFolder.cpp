#include <functional>
#include <simplecc/Transform/TrivialConstantFolder.h>

using namespace simplecc;

// TODO: don't use such a template.
template <typename BinaryFunc>
static inline int Compute(BinaryFunc Op, int L, int R) {
  return Op(L, R);
}

ExprAST *TrivialConstantFolder::FoldBinOpExpr(BinOpExpr *B) {
  ExprAST *L = B->getLeft();
  ExprAST *R = B->getRight();

  // Case-1: both side is non-constant.
  if (!L->isConstant() && !R->isConstant()) {
    // If both side are the same name, there may be opportunity.
    if (IsInstance<NameExpr>(L) && IsInstance<NameExpr>(R) &&
        static_cast<NameExpr *>(L)->getName() ==
            static_cast<NameExpr *>(R)->getName()) {
      switch (B->getOp()) {
        // Case-1-1: X - X == 0
      case BinaryOpKind::Sub:
        return new NumExpr(0, B->getLocation());
        // Case-1-2: Since X might be zero, which will cause a ZeroDivisor, we
        // lose an opportunity.
      case BinaryOpKind::Div:
        return B;
        // Case-1-3: X == X == 1, X >= X == 1, X <= X == 1
      case BinaryOpKind::GtE:
      case BinaryOpKind::LtE: // Fall through
      case BinaryOpKind::Eq:
        return new NumExpr(1, B->getLocation());
        // Case-1-4: X != X == 0, X < X == 0, X > X == 0
      case BinaryOpKind::Lt:
      case BinaryOpKind::Gt: // Fall through
      case BinaryOpKind::NotEq:
        return new NumExpr(0, B->getLocation());
      default:
        return B;
      }
    }
    // No opportunity.
    return B;
  }

  // Case-2: Zero divisor, no opportunity.
  if (B->getOp() == BinaryOpKind::Div && R->isZeroVal()) {
    // ZeroDivisorError.
    return B;
  }

  // Case-2: both side is constant, evaluate it directly.
  if (L->isConstant() && R->isConstant()) {
    switch (B->getOp()) {
    default:
      assert(false && "Unhandled Enum Value");
#define HANDLE_OPERATOR(VAL, OP, FUNC)                                         \
  case BinaryOpKind::VAL:                                                      \
    return new NumExpr(Compute(std::FUNC<int>(), L->getConstantValue(),        \
                               R->getConstantValue()),                         \
                       B->getLocation());
#include "simplecc/AST/Enums.def"
    }
  }

  // Case-3: single side constant.
  switch (B->getOp()) {
  case BinaryOpKind::Add:
    // 0 + X == X + 0 == X
    if (L->isZeroVal())
      return (std::move(*B).getRight().release());
    if (R->isZeroVal())
      return (std::move(*B).getLeft().release());
    return B;
  case BinaryOpKind::Sub:
    // 0 - X == -X
    if (L->isZeroVal())
      return new UnaryOpExpr(
          UnaryOpKind::USub,
          (std::move(*B).getRight().release()),
          B->getLocation());
    return B;
  case BinaryOpKind::Mult:
    // 0 * X == X * 0 == 0
    if (L->isZeroVal() || R->isZeroVal())
      return new NumExpr(0, B->getLocation());
    // 1 * X == X * 1 == X
    if (L->isOneVal())
      return std::move(*B).getRight().release();
    if (R->isOneVal())
      return std::move(*B).getLeft().release();
    return B;
  case BinaryOpKind::Div:
    // 0 / X == 0, but X may be zero, which will cause a ZeroDivisor. Lose
    // opportunity. X / 1 == X
    if (R->isOneVal())
      return std::move(*B).getLeft().release();
    return B;
  default:
    return B;
  }
}

ExprAST *TrivialConstantFolder::FoldUnaryOpExpr(UnaryOpExpr *U) {
  // Case-1: Ignore UAdd, +X => X
  if (U->getOp() == UnaryOpKind::UAdd) {
    return std::move(*U).getOperand().release();
  }
  // Case-2: Compute negate of constant like -1, -2.
  assert(U->getOp() == UnaryOpKind::USub);
  auto Operand = U->getOperand();
  if (Operand->isConstant()) {
    return new NumExpr(-Operand->getConstantValue(), U->getLocation());
  }

  // Case-3: fold double negate into noop, --X => X
  if (IsInstance<UnaryOpExpr>(Operand) &&
      static_cast<UnaryOpExpr *>(Operand)->getOp() == UnaryOpKind::USub) {
    auto UO = static_cast<UnaryOpExpr *>(Operand);
    return std::move(*UO).getOperand().release();
  }
  return U;
}

ExprAST *TrivialConstantFolder::FoldParenExpr(ParenExpr *P) {
  // Extract wrapped value, (X) => X
  return (std::move(*P).getValue().release());
}

ExprAST *TrivialConstantFolder::FoldNameExpr(NameExpr *N) {
  auto Entry = getSymbolEntry(N->getName());
  if (!Entry.IsConstant()) {
    return N;
  }
  auto CT = Entry.AsConstant();
  switch (CT.getType()) {
  case BasicTypeKind::Int:
    return new NumExpr(CT.getValue(), N->getLocation());
  case BasicTypeKind::Character:
    return new CharExpr(CT.getValue(), N->getLocation());
  default:
    assert(false && "Invalid Enum Value");
  }
}

ExprAST *TrivialConstantFolder::FoldExprAST(ExprAST *E) {
  switch (E->getKind()) {
#define HANDLE_CONST_FOLD(Class)                                               \
  case ExprAST::Class##Kind:                                                   \
    return Fold##Class(static_cast<Class *>(E));
#include "simplecc/Transform/TrivialConstantFolder.def"
  default:
    return E;
  }
}

ExprAST *TrivialConstantFolder::TransformExpr(ExprAST *E, AST *Parent) {
  // First use base class method to transform children of E.
  ExpressionTransformer::visitExpr(E);
  // Then fold E.
  return FoldExprAST(E);
}