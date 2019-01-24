#include "simplecc/Analysis/TypeEvaluator.h"
#include <cassert>

using namespace simplecc;

BasicTypeKind TypeEvaluator::visitSubscript(SubscriptExpr *S) {
  auto Entry = TheLocal[S->getArrayName()];
  assert(Entry.IsArray() && "invalid access to non array");
  return Entry.AsArray().getElementType();
}

BasicTypeKind TypeEvaluator::visitName(NameExpr *N) {
  // TODO: fix the mixture of SymbolEntry and type judgement.
  auto Entry = TheLocal[N->getName()];
  if (Entry.IsVariable())
    return Entry.AsVariable().getType();
  if (Entry.IsConstant())
    return Entry.AsConstant().getType();
  assert(false && "Bad type for NameExpr");
}

BasicTypeKind TypeEvaluator::visitCall(CallExpr *C) {
  auto Entry = TheLocal[C->getCallee()];
  assert(Entry.IsFunction() && "invalid access to non function");
  return Entry.AsFunction().getReturnType();
}

BasicTypeKind TypeEvaluator::getExprType(const ExprAST *E) const {
  return const_cast<TypeEvaluator *>(this)->visitExpr(const_cast<ExprAST *>(E));
}
