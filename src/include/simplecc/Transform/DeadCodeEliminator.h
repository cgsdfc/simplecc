#ifndef SIMPLECC_TRANSFORM_TRIVIALDCETRANSFORMER_H
#define SIMPLECC_TRANSFORM_TRIVIALDCETRANSFORMER_H
#include "simplecc/Analysis/Visitor.h"

namespace simplecc {
/// This class implements a trivial dead code elimination (DCE) transformation
/// on the AST. It basically performs these operations:
/// 1. Delete stmts in a stmt list that appear after a return-stmt.
/// 2. For an if-stmt, if its condition is a constant, then replace it with either branch
/// that will actually execute.
/// 3. For a while-stmt, if its condition is false, delete it.
/// 4. For a for-stmt, if its condition is false, do the following:
/// for (initial; condition; step) { body } becomes:
/// initial; body; step;
/// This transformation requires the TrivialConstantFolder to run first.
class DeadCodeEliminator : ChildrenVisitor<DeadCodeEliminator> {
  friend VisitorBase;
  friend ChildrenVisitor;
  using StmtListType = std::vector<StmtAST *>;
  /// Run the DCE algorithm once on the stmt list, return if
  /// the list was mutated.
  void TransformStmtList(StmtListType &StmtList);
  void visitStmtList(StmtListType &StmtList);
  void visitFor(ForStmt *F);
  void visitWhile(WhileStmt *W);
  void visitIf(IfStmt *I);
  void visitFuncDef(FuncDef *FD);

public:
  DeadCodeEliminator() = default;
  void Transform(Program *P) {
    return ChildrenVisitor::visitProgram(P);
  }
};
}
#endif //SIMPLECC_TRANSFORM_TRIVIALDCETRANSFORMER_H
