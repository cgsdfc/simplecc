#ifndef SIMPLECC_TRANSFORM_TRIVIALCONSTANTFOLDER_H
#define SIMPLECC_TRANSFORM_TRIVIALCONSTANTFOLDER_H
#include "simplecc/Transform/ExpressionTransformer.h"

namespace simplecc {

/// This class performs trivial constant folding on Expr nodes.
class TrivialConstantFolder : ExpressionTransformer<TrivialConstantFolder> {
  friend ExpressionTransformer;
  ExprAST *FoldBinOpExpr(BinOpExpr *B);
  ExprAST *FoldUnaryOpExpr(UnaryOpExpr *U);
  ExprAST *FoldParenExpr(ParenExpr *P);
  ExprAST *FoldExprAST(ExprAST *E);
  ExprAST *TransformExpr(ExprAST *E, AST *Parent);
public:
  TrivialConstantFolder() = default;
  using ExpressionTransformer::Transform;
};

}
#endif //SIMPLECC_TRANSFORM_TRIVIALCONSTANTFOLDER_H