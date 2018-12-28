#include "simplecc/Transform/ConstantInliner.h"
#include <algorithm> // for remove_if

using namespace simplecc;

Expr *ConstantInliner::TransformExpr(Expr *E) {
  if (!IsInstance<NameExpr>(E)) {
    visitExpr(E);
    return E;
  }
  SymbolEntry Entry(TheLocalTable[static_cast<NameExpr *>(E)->getId()]);
  if (!Entry.IsConstant()) {
    visitExpr(E);
    return E;
  }
  ConstType CT(Entry.AsConstant());
  switch (CT.getType()) {
  case BasicTypeKind::Int:return new NumExpr(CT.getValue(), E->getLocation());
  case BasicTypeKind::Character:return new CharExpr(CT.getValue(), E->getLocation());
  default:assert(false && "Invalid Enum Value");
  }
}

void ConstantInliner::deleteConstDecls(std::vector<Decl *> &DeclList) {
  auto CDBegin = std::find_if(DeclList.begin(), DeclList.end(), IsInstance<ConstDecl, Decl>);
  if (CDBegin == DeclList.end())
    return;
  auto CDEnd = std::find(CDBegin, DeclList.end(), IsInstance<ConstDecl, Decl>);
  assert(CDEnd != DeclList.end());
  std::next(CDEnd);
  std::for_each(CDBegin, CDEnd, DeleteAST());
  DeclList.erase(CDBegin, CDEnd);
}