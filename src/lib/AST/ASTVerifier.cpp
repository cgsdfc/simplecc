// MIT License
// 
// Copyright (c) 2018 Cong Feng.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "simplecc/AST/ASTVerifier.h"

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

bool ASTVerifier::Check(ProgramAST *P) {
  EM.setErrorType("InternalError");
  visitProgram(P);
  return !EM.IsOk();
}

void ASTVerifier::AssertThat(bool Predicate, const char *ErrMsg) {
  if (Predicate)
    return;
  EM.Error(ErrMsg);
}