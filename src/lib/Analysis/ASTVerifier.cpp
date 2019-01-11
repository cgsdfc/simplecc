#include "simplecc/Analysis/ASTVerifier.h"

using namespace simplecc;

void ASTVerifier::visitWrite(WriteStmt *WR) {
  AssertThat(WR->getStr() || WR->getValue(),
             "Both StrExpr and Value of WriteStmt are empty");
}

void ASTVerifier::visitWhile(WhileStmt *W) {
  AssertThat(IsInstance<BoolOpExpr>(W->getCondition()),
             "Condition of WhileStmt must be a BoolOpExpr");
}

void ASTVerifier::visitConstDecl(ConstDecl *CD) {
  auto Val = CD->getValue();
  AssertThat(IsInstance<CharExpr>(Val) || IsInstance<NumExpr>(Val),
             "Value of ConstDecl must be NumExpr or CharExpr");
}

void ASTVerifier::visitExprStmt(ExprStmt *ES) {
  AssertThat(IsInstance<CallExpr>(ES->getValue()),
             "ExprStmt must have a CallExpr");
}

void ASTVerifier::visitIf(IfStmt *I) {
  AssertThat(IsInstance<BoolOpExpr>(I->getCondition()),
             "Test of IfStmt must be a BoolOpExpr");
}

void ASTVerifier::visitBoolOp(BoolOpExpr *B) {
  if (B->hasCompareOp()) {
    AssertThat(IsInstance<BinOpExpr>(B->getValue()),
               "HasCmpOp implies BinOpExpr");
  }
}

void ASTVerifier::visitAssign(AssignStmt *A) {
  AssertThat(IsInstance<NameExpr>(A->getTarget()) ||
                 IsInstance<SubscriptExpr>(A->getTarget()),
             "Target of AssignStmt must be NameExpr or SubscriptExpr");
}

void ASTVerifier::visitFor(ForStmt *F) {
  AssertThat(IsInstance<AssignStmt>(F->getInitial()),
             "Initial of ForStmt must be an AssignStmt");

  AssertThat(IsInstance<BoolOpExpr>(F->getCondition()),
             "Condition of ForStmt must be a BoolOpExpr");

  AssertThat(IsInstance<AssignStmt>(F->getStep()),
             "Step of ForStmt must be an AssignStmt");
}

void ASTVerifier::visitFuncDef(FuncDef *FD) {
  for (DeclAST *D : FD->getDecls()) {
    AssertThat(IsInstance<ConstDecl>(D) || IsInstance<VarDecl>(D),
               "Decls of FuncDef must be ConstDecl or VarDecl");
  }
  for (StmtAST *S : FD->getStmts()) {
    visitStmt(S);
  }
}

void ASTVerifier::visitProgram(ProgramAST *P) {
  for (DeclAST *D : P->getDecls()) {
    AssertThat(!IsInstance<ArgDecl>(D),
               "ArgDecl cannot appear in Decls of ProgramAST");
    visitDecl(D);
  }
}

bool ASTVerifier::Verify(ProgramAST *P) {
  EM.setErrorType("InternalError");
  visitProgram(P);
  return !EM.IsOk();
}

void ASTVerifier::AssertThat(bool Predicate, const char *ErrMsg) {
  if (Predicate)
    return;
  EM.Error(ErrMsg);
}