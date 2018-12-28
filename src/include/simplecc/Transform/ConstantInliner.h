#ifndef SIMPLECC_TRANSFORM_CONSTANTINLINER_H
#define SIMPLECC_TRANSFORM_CONSTANTINLINER_H
#include "simplecc/Analysis/Visitor.h"
#include "simplecc/Analysis/SymbolTable.h"

namespace simplecc {

/// This class transforms all the use of ConstDecl's into the corresponding literal
/// value, e.g. const char C = 'a'; Printf(C); becomes Printf('a');
/// It then deletes all the ConstDecl nodes from their parents along with their entries in
/// the SymbolTable.
class ConstantInliner : ChildrenVisitor<ConstantInliner> {
  /// If E is a NameExpr and corresponds to a ConstType, create a corresponding literal
  /// node (CharExpr or NumExpr). If not, E is visited and then return as it.
  Expr *TransformExpr(Expr *E);
  void deleteConstDecls(std::vector<Decl *> &DeclList);

private:
  friend class ChildrenVisitor<ConstantInliner>;
  SymbolTable *TheTable;
  LocalSymbolTable TheLocalTable;
};
}

#endif //SIMPLECC_TRANSFORM_CONSTANTINLINER_H