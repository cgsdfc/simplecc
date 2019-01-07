#ifndef SIMPLCC_ANALYSIS_ASTVERIFIER_H
#define SIMPLCC_ANALYSIS_ASTVERIFIER_H
#include "simplecc/Analysis/Visitor.h"
#include "simplecc/Support/ErrorManager.h"

namespace simplecc {
/// This class verifies that a Program is well formed.
class ASTVerifier : ChildrenVisitor<ASTVerifier> {
  /// Helper to check a condition.
  void AssertThat(bool Predicate, const char *ErrMsg);

  void visitRead(ReadStmt *RD);
  void visitWrite(WriteStmt *WR);
  void visitAssign(AssignStmt *A);
  void visitBoolOp(BoolOpExpr *B);
  void visitExprStmt(ExprStmt *ES);
  void visitConstDecl(ConstDecl *CD);
  void visitFor(ForStmt *F);
  void visitWhile(WhileStmt *W);
  void visitIf(IfStmt *I);
  void visitFuncDef(FuncDef *FD);
  void visitProgram(ProgramAST *P);
public:
  ASTVerifier() = default;
  ~ASTVerifier() = default;
  bool Verify(ProgramAST *P);
private:
  friend ChildrenVisitor;
  friend VisitorBase;
  ErrorManager EM;
};
} // namespace simplecc
#endif