#include "simplecc/Analysis/AstVerifier.h"

using namespace simplecc;

void AstVerifier::visitWrite(simplecc::Write *WR) {
  AssertThat(WR->getStr() || WR->getValue(),
             "Both Str and Value of Write are empty");

}

void AstVerifier::visitWhile(While *W) {
  AssertThat(IsInstance<BoolOp>(W->getCondition()),
             "Condition of While must be a BoolOp");
}

void AstVerifier::visitConstDecl(ConstDecl *CD) {
  auto Val = CD->getValue();
  AssertThat(IsInstance<Char>(Val) || IsInstance<Num>(Val),
             "Value of ConstDecl must be Num or Char");
}

void AstVerifier::visitExprStmt(ExprStmt *ES) {
  AssertThat(IsInstance<Call>(ES->getValue()), "ExprStmt must have a Call");
}

void AstVerifier::visitIf(If *I) {
  AssertThat(IsInstance<BoolOp>(I->getTest()), "Test of If must be a BoolOp");
}

void AstVerifier::visitBoolOp(simplecc::BoolOp *B) {
  if (B->getHasCmpop()) {
    AssertThat(IsInstance<BinOp>(B->getValue()), "HasCmpOp implies BinOp");
  }
}

void AstVerifier::visitRead(Read *RD) {
  for (Expr *E : RD->getNames()) {
    AssertThat(IsInstance<Name>(E), "Names in Read must be Name");
  }
}

void AstVerifier::visitAssign(simplecc::Assign *A) {
  AssertThat(IsInstance<Name>(A->getTarget()) ||
                 IsInstance<Subscript>(A->getTarget()),
             "Target of Assign must be Name or Subscript");

}

void AstVerifier::visitFor(For *F) {
  AssertThat(IsInstance<Assign>(F->getInitial()),
             "Initial of For must be an Assign");

  AssertThat(IsInstance<BoolOp>(F->getCondition()),
             "Condition of For must be a BoolOp");

  AssertThat(IsInstance<Assign>(F->getStep()),
             "Step of For must be an Assign");
}

void AstVerifier::visitFuncDef(FuncDef *FD) {
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

void AstVerifier::visitProgram(simplecc::Program *P) {
  for (Decl *D : P->getDecls()) {
    AssertThat(!IsInstance<ArgDecl>(D),
               "ArgDecl cannot appear in Decls of Program");
    visitDecl(D);
  }
}

bool AstVerifier::Verify(Program *P) {
  EM.setErrorType("InternalError");
  visitProgram(P);
  return EM.IsOk();
}

namespace simplecc {
bool VerifyAST(Program *P) { return AstVerifier().Verify(P); }
} // namespace simplecc
