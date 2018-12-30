#ifndef SIMPLECC_TRANSFORM_INLINECONSTDECLTRANSFORMER_H
#define SIMPLECC_TRANSFORM_INLINECONSTDECLTRANSFORMER_H
#include "simplecc/Transform/ExprTransformer.h"

namespace simplecc {

/// This class transforms all the use of ConstDecl's into the corresponding literal
/// value, e.g. const char C = 'a'; Printf(C); becomes Printf('a');
/// It then deletes all the ConstDecl nodes from their parents along with their entries in
/// the SymbolTable.
class InlineConstDeclTransformer : ExprTransformer<InlineConstDeclTransformer> {
  /// If E is a NameExpr and corresponds to a ConstType, create a corresponding literal
  /// node (CharExpr or NumExpr). If not, E is visited and then return as it.
  Expr *TransformExpr(Expr *E, AST *);
private:
  friend ExprTransformer;
};
}

#endif //SIMPLECC_TRANSFORM_INLINECONSTDECLTRANSFORMER_H