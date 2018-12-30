#ifndef SIMPLECC_TRANSFORM_INLINECONSTDECLTRANSFORMER_H
#define SIMPLECC_TRANSFORM_INLINECONSTDECLTRANSFORMER_H
#include "simplecc/Transform/ExpressionTransformer.h"

namespace simplecc {

/// This class transforms all the use of ConstDecl's into the corresponding literal
/// value. For example,
/// const char C = 'a';
/// Printf(C);
/// becomes
/// Printf('a');
/// It then deletes all the ConstDecl nodes from their parents along with their entries in
/// the SymbolTable.
class InlineConstDeclTransformer : ExpressionTransformer<InlineConstDeclTransformer> {
  friend ExpressionTransformer;
  /// If E is a NameExpr and corresponds to a ConstType, create a corresponding literal
  /// node (CharExpr or NumExpr).
  Expr *TransformExpr(Expr *E, AST *Parent);
public:
  InlineConstDeclTransformer() = default;
  using ExpressionTransformer::Transform;
};
}

#endif //SIMPLECC_TRANSFORM_INLINECONSTDECLTRANSFORMER_H