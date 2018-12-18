#ifndef AST_VERIFIER_H
#define AST_VERIFIER_H
#include "simplecc/Visitor.h"
#include "simplecc/ErrorManager.h"

namespace simplecc {
class Program;
/// This ensure that the AST is well-formed.
/// In terms of well-formness:
class AstVerifier : ChildrenVisitor<AstVerifier> {

  /// Helper to check each condition in one single statement.
  void AssertThat(bool Pred, const char *ErrMsg) {
    if (Pred)
      return;
    EM.Error(ErrMsg);
  }

  void visitRead(Read *RD);

  void visitWrite(Write *WR);

  void visitAssign(Assign *A);

  void visitBoolOp(BoolOp *B);

  void visitExprStmt(ExprStmt *ES);

  void visitConstDecl(ConstDecl *CD);

  void visitFor(For *F);

  void visitWhile(While *W);

  void visitIf(If *I);

  void visitFuncDef(FuncDef *FD);

  void visitProgram(Program *P);

public:
  AstVerifier() = default;
  ~AstVerifier() = default;

  bool Verify(Program *P);

private:
  friend class ChildrenVisitor<AstVerifier>;
  friend class VisitorBase<AstVerifier>;
  ErrorManager EM;
};

bool VerifyAST(Program *P);
} // namespace simplecc
#endif
