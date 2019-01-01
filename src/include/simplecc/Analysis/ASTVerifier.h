#ifndef SIMPLCC_ANALYSIS_ASTVERIFIER_H
#define SIMPLCC_ANALYSIS_ASTVERIFIER_H
#include "simplecc/Analysis/Visitor.h"
#include "simplecc/Support/ErrorManager.h"

namespace simplecc {
/// This ensure that the AST is well-formed.
/// In terms of well-form-ness:
class ASTVerifier : ChildrenVisitor<ASTVerifier> {
  /// Helper to check each condition in one single statement.
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

  void visitProgram(Program *P);

public:
  ASTVerifier() = default;
  ~ASTVerifier() = default;

  bool Verify(Program *P);

private:
  friend ChildrenVisitor;
  friend VisitorBase;
  ErrorManager EM;
};
} // namespace simplecc
#endif
