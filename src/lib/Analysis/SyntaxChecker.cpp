#include "simplecc/Analysis/SyntaxChecker.h"
#include <cassert>
#include <simplecc/Analysis/SyntaxChecker.h>

using namespace simplecc;

void SyntaxChecker::visitProgram(ProgramAST *P) {
  if (P->getDecls().empty()) {
    EM.Error("empty input is invalid. A main function is required at minimum");
    return;
  }

  // Check the order of declarations.
  int PrevDecl = DeclAST::ConstDeclKind;
  for (auto D : P->getDecls()) {
    switch (D->getKind()) {
    case DeclAST::ConstDeclKind:
      if (PrevDecl != DeclAST::ConstDeclKind) {
        // ConstDecl can only be preceded by ConstDecl.
        EM.Error(D->getLocation(), "unexpected const declaration");
      }
      break;
    case DeclAST::VarDeclKind:
      if (PrevDecl == DeclAST::FuncDefKind) {
        // VarDecl cannot be preceded by FuncDef.
        EM.Error(D->getLocation(), "unexpected variable declaration");
      }
      break;
    case DeclAST::FuncDefKind:
      // FuncDef can be preceded by anything.
      break;
    default:assert(false && "Impossible DeclAST Kind");
    }
    PrevDecl = D->getKind();
    VisitorBase::visitDecl(D);
  }

  // check the last declaration is the void main() function
  DeclAST *LastDecl = P->getDecls().back();
  if (isMainFunction(LastDecl))
    return;
  EM.Error(LastDecl->getLocation(), "the last declaration must be void main()");
}

void SyntaxChecker::visitConstDecl(ConstDecl *CD) {
  if (CD->getType() == BasicTypeKind::Int &&
      !IsInstance<NumExpr>(CD->getValue())) {
    EM.Error(CD->getLocation(), "expected int initializer");
  }

  if (CD->getType() == BasicTypeKind::Character &&
      !IsInstance<CharExpr>(CD->getValue())) {
    EM.Error(CD->getLocation(), "expected char initializer");
  }
}

void SyntaxChecker::visitVarDecl(VarDecl *VD) {
  if (VD->getType() == BasicTypeKind::Void) {
    EM.Error(VD->getLocation(), "cannot declare void variable");
  }

  if (VD->isArray() && VD->getSize() == 0) {
    EM.Error(VD->getLocation(), "array size cannot be 0");
  }
}

void SyntaxChecker::visitFuncDef(FuncDef *FD) {
  for (auto A : FD->getArgs()) {
    visitArgDecl(A);
  }
  for (auto D : FD->getDecls()) {
    VisitorBase::visitDecl(D);
  }
}

void SyntaxChecker::visitArgDecl(ArgDecl *AD) {
  if (AD->getType() == BasicTypeKind::Void) {
    EM.Error(AD->getLocation(), "cannot declare void argument");
  }
}

bool SyntaxChecker::Check(ProgramAST *P) {
  visitProgram(P);
  return !EM.IsOk();
}

SyntaxChecker::SyntaxChecker() {
  EM.setErrorType("SyntaxError");
}

bool SyntaxChecker::isMainFunction(DeclAST *D) const {
  if (!IsInstance<FuncDef>(D))
    return false;
  auto FD = static_cast<FuncDef *>(D);
  if (FD->getName() != "main")
    return false;
  if (FD->getReturnType() != BasicTypeKind::Void)
    return false;
  return FD->getNumArgs() == 0;
}
