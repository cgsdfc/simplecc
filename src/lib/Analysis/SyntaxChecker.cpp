#include "simplecc/Analysis/SyntaxChecker.h"
#include <cassert>

using namespace simplecc;

void SyntaxChecker::visitProgram(Program *P) {
  if (P->getDecls().empty()) {
    EM.Error("empty input is invalid. A main function is required at minimum");
    return;
  }

  // Check the order of declarations.
  int PrevDecl = Decl::ConstDecl;
  for (auto D : P->getDecls()) {
    switch (D->GetKind()) {
    case Decl::ConstDecl:
      if (PrevDecl != Decl::ConstDecl) {
        // ConstDecl can only be preceded by ConstDecl.
        EM.Error(D->getLoc(), "unexpected const declaration");
      }
      break;
    case Decl::VarDecl:
      if (PrevDecl == Decl::FuncDef) {
        // VarDecl cannot be preceded by FuncDef.
        EM.Error(D->getLoc(), "unexpected variable declaration");
      }
      break;
    case Decl::FuncDef:
      // FuncDef can be preceded by anything.
      break;
    default:
      assert(false && "Impossible Decl Kind");
    }
    PrevDecl = D->GetKind();
    VisitorBase::visitDecl(D);
  }

  // check the last declaration is the void main() function
  Decl *LastDecl = P->getDecls().back();
  if (IsInstance<FuncDef>(LastDecl) && LastDecl->getName() == "main" &&
      static_cast<FuncDef *>(LastDecl)->getReturnType() == BasicTypeKind::Void)
    return;

  EM.Error(LastDecl->getLoc(), "the last declaration must be void main()");
}

void SyntaxChecker::visitConstDecl(ConstDecl *CD) {
  if (CD->getType() == BasicTypeKind::Int && !IsInstance<NumExpr>(CD->getValue())) {
    EM.Error(CD->getLoc(), "expected int initializer");
  }

  if (CD->getType() == BasicTypeKind::Character &&
      !IsInstance<CharExpr>(CD->getValue())) {
    EM.Error(CD->getLoc(), "expected char initializer");
  }
}

void SyntaxChecker::visitVarDecl(VarDecl *VD) {
  if (VD->getType() == BasicTypeKind::Void) {
    EM.Error(VD->getLoc(), "cannot declare void variable");
  }
  if (VD->getIsArray() && VD->getSize() == 0) {
    EM.Error(VD->getLoc(), "array size cannot be 0");
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
    EM.Error(AD->getLoc(), "cannot declare void argument");
  }
}

bool SyntaxChecker::Check(Program *P) {
  visitProgram(P);
  return !EM.IsOk();
}