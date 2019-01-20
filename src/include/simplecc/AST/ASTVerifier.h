#ifndef SIMPLECC_AST_ASTVERIFIER_H
#define SIMPLECC_AST_ASTVERIFIER_H
#include "simplecc/AST/ChildrenVisitor.h"
#include "simplecc/Support/ErrorManager.h"

namespace simplecc {
/// @brief ASTVerifier verifies that various AST nodes can only be of a fixed range of subclasses.
/// These conditions cannot be enforced by a C++ compiler although the much the better.
/// Example is the Target of an AssignStmt can only be a NameExpr or SubscriptExpr, not other things.
/// The constrains are imposed by the language specification.
class ASTVerifier : ChildrenVisitor<ASTVerifier> {
  /// Helper to check a condition.
  void AssertThat(bool Predicate, const char *ErrMsg);

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
  /// Check a program.
  bool Check(ProgramAST *P);
private:
  friend ChildrenVisitor;
  friend VisitorBase;
  ErrorManager EM;
};
} // namespace simplecc
#endif // SIMPLECC_AST_ASTVERIFIER_H