#include "simplecc/Analysis/SyntaxChecker.h"
#include <cassert>

using namespace simplecc;

void SyntaxChecker::visitProgram(Program *P) {
  if (P->getDecls().empty()) {
    EM.Error("empty input is invalid. A main function is required at minimum");
    return;
  }

  // Check the order of declarations.
  int PrevDecl = Decl::ConstDeclKind;
  for (auto D : P->getDecls()) {
    switch (D->getKind()) {
    case Decl::ConstDeclKind:
      if (PrevDecl != Decl::ConstDeclKind) {
        // ConstDecl can only be preceded by ConstDecl.
        EM.Error(D->getLocation(), "unexpected const declaration");
      }
      break;
    case Decl::VarDeclKind:
      if (PrevDecl == Decl::FuncDefKind) {
        // VarDecl cannot be preceded by FuncDef.
        EM.Error(D->getLocation(), "unexpected variable declaration");
      }
      break;
    case Decl::FuncDefKind:
      // FuncDef can be preceded by anything.
      break;
    default:
      assert(false && "Impossible Decl Kind");
    }
    PrevDecl = D->getKind();
    VisitorBase::visitDecl(D);
  }

  // check the last declaration is the void main() function
  Decl *LastDecl = P->getDecls().back();
  if (IsInstance<FuncDef>(LastDecl) && LastDecl->getName() == "main" &&
      static_cast<FuncDef *>(LastDecl)->getReturnType() == BasicTypeKind::Void)
    return;

  EM.Error(LastDecl->getLocation(), "the last declaration must be void main()");
}

void SyntaxChecker::visitConstDecl(ConstDecl *CD) {
  if (CD->getType() == BasicTypeKind::Int && !IsInstance<NumExpr>(CD->getValue())) {
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
  if (VD->getIsArray() && VD->getSize() == 0) {
    EM.Error(VD->getLocation(), "array size cannot be 0");
  }
}

void SyntaxChecker::visitFuncDef(FuncDef *FD) {
  for (auto A : FD->getArgs()) {
    VisitorBase::visitDecl(A);
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

bool SyntaxChecker::Check(Program *P) {
  visitProgram(P);
  return !EM.IsOk();
}