#include "simplecc/Analysis/AstVerifier.h"

using namespace simplecc;

void AstVerifier::visitWrite(simplecc::WriteStmt *WR) {
  AssertThat(WR->getStr() || WR->getValue(),
             "Both StrExpr and Value of WriteStmt are empty");
}

void AstVerifier::visitWhile(WhileStmt *W) {
  AssertThat(IsInstance<BoolOpExpr>(W->getCondition()),
             "Condition of WhileStmt must be a BoolOpExpr");
}

void AstVerifier::visitConstDecl(ConstDecl *CD) {
  auto Val = CD->getValue();
  AssertThat(IsInstance<CharExpr>(Val) || IsInstance<NumExpr>(Val),
             "Value of ConstDecl must be NumExpr or CharExpr");
}

void AstVerifier::visitExprStmt(ExprStmt *ES) {
  AssertThat(IsInstance<CallExpr>(ES->getValue()),
             "ExprStmt must have a CallExpr");
}

void AstVerifier::visitIf(IfStmt *I) {
  AssertThat(IsInstance<BoolOpExpr>(I->getTest()),
             "Test of IfStmt must be a BoolOpExpr");
}

void AstVerifier::visitBoolOp(simplecc::BoolOpExpr *B) {
  if (B->hasCompareOp()) {
    AssertThat(IsInstance<BinOpExpr>(B->getValue()),
               "HasCmpOp implies BinOpExpr");
  }
}

void AstVerifier::visitRead(ReadStmt *RD) {
  for (Expr *E : RD->getNames()) {
    AssertThat(IsInstance<NameExpr>(E), "Names in ReadStmt must be NameExpr");
  }
}

void AstVerifier::visitAssign(simplecc::AssignStmt *A) {
  AssertThat(IsInstance<NameExpr>(A->getTarget()) ||
                 IsInstance<SubscriptExpr>(A->getTarget()),
             "Target of AssignStmt must be NameExpr or SubscriptExpr");
}

void AstVerifier::visitFor(ForStmt *F) {
  AssertThat(IsInstance<AssignStmt>(F->getInitial()),
             "Initial of ForStmt must be an AssignStmt");

  AssertThat(IsInstance<BoolOpExpr>(F->getCondition()),
             "Condition of ForStmt must be a BoolOpExpr");

  AssertThat(IsInstance<AssignStmt>(F->getStep()),
             "Step of ForStmt must be an AssignStmt");
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
  return !EM.IsOk();
}

void AstVerifier::AssertThat(bool Predicate, const char *ErrMsg) {
  if (Predicate)
    return;
  EM.Error(ErrMsg);
}