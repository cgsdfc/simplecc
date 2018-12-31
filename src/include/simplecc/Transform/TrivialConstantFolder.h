#ifndef SIMPLECC_TRANSFORM_TRIVIALCONSTANTFOLDER_H
#define SIMPLECC_TRANSFORM_TRIVIALCONSTANTFOLDER_H
#include "simplecc/Transform/ExpressionTransformer.h"

namespace simplecc {

/// This class performs trivial constant folding on Expr nodes.
class TrivialConstantFolder : ExpressionTransformer<TrivialConstantFolder> {
  friend ExpressionTransformer;
  Expr *visitBinOp(BinOpExpr *B);
  Expr *visitUnaryOp(UnaryOpExpr *U);
  Expr *visitParenExpr(ParenExpr *P);
  Expr *visitExpr(Expr *E);

  Expr *TransformExpr(Expr *E, AST *Parent);
public:
  TrivialConstantFolder() = default;
  using ExpressionTransformer::Transform;
};

}
#endif //SIMPLECC_TRANSFORM_TRIVIALCONSTANTFOLDER_H
