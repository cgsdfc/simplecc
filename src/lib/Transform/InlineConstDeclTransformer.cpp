#include "simplecc/Transform/InlineConstDeclTransformer.h"

using namespace simplecc;

Expr *InlineConstDeclTransformer::TransformExpr(Expr *E, AST *Parent) {
  if (!IsInstance<NameExpr>(E)) {
    return ExprTransformer::TransformExpr(E, Parent);
  }
  auto Entry = getSymbolEntry(static_cast<NameExpr *>(E)->getId());
  if (!Entry.IsConstant()) {
    return ExprTransformer::TransformExpr(E, Parent);
  }
  auto CT = Entry.AsConstant();
  switch (CT.getType()) {
  case BasicTypeKind::Int:
    return new NumExpr(CT.getValue(), E->getLocation());
  case BasicTypeKind::Character:
    return new CharExpr(CT.getValue(), E->getLocation());
  default:
    assert(false && "Invalid Enum Value");
  }
}