#include "AstVerifier.h"
#include "ErrorManager.h"
#include "Visitor.h"

namespace simplecompiler {
class AstVerifier : ChildrenVisitor<AstVerifier> {

  /// Helper to check each condition in one single statement.
  void AssertThat(bool Pred, const char *ErrMsg) {
    if (Pred)
      return;
    EM.InternalError(ErrMsg);
  }

  void visitRead(Read *RD) {
    for (Expr *E : RD->getNames()) {
      AssertThat(IsInstance<Name>(E), "Names in Read must be Name");
    }
  }

  void visitWrite(Write *WR) {
    AssertThat(WR->getStr() || WR->getValue(),
               "Both Str and Value of Write are empty");
  }

  void visitAssign(Assign *A) {
    AssertThat(IsInstance<Name>(A->getTarget()) ||
                   IsInstance<Subscript>(A->getTarget()),
               "Target of Assign must be Name or Subscript");
  }

  void visitBoolOp(BoolOp *B) {
    if (B->getHasCmpop()) {
      AssertThat(IsInstance<BinOp>(B->getValue()), "HasCmpOp implies BinOp");
    }
  }

  void visitExprStmt(ExprStmt *ES) {
    AssertThat(IsInstance<Call>(ES->getValue()), "ExprStmt must have a Call");
  }

  void visitConstDecl(ConstDecl *CD) {
    auto Val = CD->getValue();
    AssertThat(IsInstance<Char>(Val) || IsInstance<Num>(Val),
               "Value of ConstDecl must be Num or Char");
  }

  void visitFor(For *F) {
    AssertThat(IsInstance<Assign>(F->getInitial()),
               "Initial of For must be an Assign");

    AssertThat(IsInstance<BoolOp>(F->getCondition()),
               "Condition of For must be a BoolOp");

    AssertThat(IsInstance<Assign>(F->getStep()),
               "Step of For must be an Assign");
  }

  void visitWhile(While *W) {
    AssertThat(IsInstance<BoolOp>(W->getCondition()),
               "Condition of While must be a BoolOp");
  }

  void visitIf(If *I) {
    AssertThat(IsInstance<BoolOp>(I->getTest()), "Test of If must be a BoolOp");
  }

  void visitFuncDef(FuncDef *FD) {
    for (Decl *D : FD->getArgs()) {
      AssertThat(IsInstance<ArgDecl>(D), "Args of FuncDef must be ArgDecl's");
    }
    for (Decl *D : FD->getDecls()) {
      AssertThat(IsInstance<ConstDecl>(D) || IsInstance<VarDecl>(D),
                 "Decls of FuncDef must be ConstDecl or VarDecl");
    }
    for (Stmt *S : FD->getStmts()) {
      visitStmt(S);
    }
  }

  void visitProgram(Program *P) {
    for (Decl *D : P->getDecls()) {
      AssertThat(!IsInstance<ArgDecl>(D),
                 "ArgDecl cannot appear in Decls of Program");
      visitDecl(D);
    }
  }

  /// VisitorBase's boilerplates.
  /* void visitExpr(Expr *E) { VisitorBase::visitExpr<void>(E); } */
  /* void visitStmt(Stmt *S) { return VisitorBase::visitStmt<void>(S); } */
  /* void visitDecl(Decl *D) { return VisitorBase::visitDecl<void>(D); } */

public:
  AstVerifier() = default;
  ~AstVerifier() = default;

  bool Verify(Program *P) {
    visitProgram(P);
    return EM.IsOk();
  }

private:
  friend class ChildrenVisitor<AstVerifier>;
  friend class VisitorBase<AstVerifier>;
  ErrorManager EM;
};

bool VerifyAST(Program *P) { return AstVerifier().Verify(P); }

} // namespace simplecompiler
