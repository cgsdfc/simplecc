#include "simplecc/Analysis/SyntaxChecker.h"

using namespace simplecc;

void SyntaxChecker::visitProgram(Program *P) {
  if (P->getDecls().size() == 0) {
    EM.Error(Location(0, 0), "expected main() at the end of input");
    return;
  }
  for (auto D : P->getDecls()) {
    VisitorBase::visitDecl(D);
  }

  auto DeclIter = P->getDecls().begin();
  auto DeclEnd = P->getDecls().end();

  // check the order of declarations
  while (DeclIter != DeclEnd && IsInstance<ConstDecl>(*DeclIter)) {
    ++DeclIter;
  }
  while (DeclIter != DeclEnd && IsInstance<VarDecl>(*DeclIter)) {
    ++DeclIter;
  }
  while (DeclIter != DeclEnd && IsInstance<FuncDef>(*DeclIter)) {
    ++DeclIter;
  }

  if (DeclIter != DeclEnd) {
    auto D = *DeclIter;
    EM.Error(D->getLoc(), "unexpected", D->GetClassName(),
                   Quote(D->getName()));
  }

  // check the last declaration is the main function
  auto FD = subclass_cast<FuncDef>(P->getDecls().back());
  if (!(FD && FD->getName() == "main")) {
    EM.Error(FD->getLoc(), "expected main() at the end of input");
  }
}

void SyntaxChecker::visitConstDecl(ConstDecl *CD) {
  {
    if (CD->getType() == BasicTypeKind::Int &&
        !IsInstance<Num>(CD->getValue())) {
      EM.Error(CD->getLoc(), "const int", Quote(CD->getName()),
                     "expects an integer");
    } else if (CD->getType() == BasicTypeKind::Character &&
        !IsInstance<Char>(CD->getValue())) {
      EM.Error(CD->getLoc(), "const char", Quote(CD->getName()),
                     "expects a character");
    }
  }
}

void SyntaxChecker::visitVarDecl(VarDecl *VD) {
  if (VD->getType() == BasicTypeKind::Void) {
    EM.Error(VD->getLoc(), "cannot declare", Quote(VD->getName()),
                   "as a void variable");
  } else if (VD->getIsArray() && VD->getSize() == 0) {
    EM.Error(VD->getLoc(), "array size of", Quote(VD->getName()),
                   "cannot be 0");
  }
}

void SyntaxChecker::visitFuncDef(FuncDef *FD) {
  for (auto Arg : FD->getArgs()) {
    VisitorBase::visitDecl(Arg);
  }
  for (auto D : FD->getDecls()) {
    VisitorBase::visitDecl(D);
  }

  if (FD->getName() == "main") {
    if (FD->getReturnType() != BasicTypeKind::Void) {
      EM.Error(FD->getLoc(), "main() must return void");
    }
  }
}

void SyntaxChecker::visitArgDecl(ArgDecl *AD) {
  if (AD->getType() == BasicTypeKind::Void) {
    EM.Error(AD->getLoc(), "cannot declare void argument");
  }
}

bool SyntaxChecker::Check(Program *P) {
  visitProgram(P);
  return EM.IsOk();
}

namespace simplecc {
bool CheckSyntax(Program *P) { return SyntaxChecker().Check(P); }
}
