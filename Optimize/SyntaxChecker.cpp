#include "SyntaxChecker.h"
#include "ErrorManager.h"
#include "Visitor.h"

namespace {
using namespace simplecompiler;

class SyntaxChecker : private VisitorBase<SyntaxChecker> {
  ErrorManager EM;
  friend class VisitorBase<SyntaxChecker>;

  void visitProgram(Program *P) {
    if (P->getDecls().size() == 0) {
      EM.SyntaxError(Location(0, 0), "expected main() at the end of input");
      return;
    }
    for (auto decl : P->getDecls()) {
      VisitorBase::visitDecl(decl);
    }

    auto decl_iter = P->getDecls().begin();
    auto end = P->getDecls().end();

    // check the order of declarations
    while (decl_iter != end && IsInstance<ConstDecl>(*decl_iter)) {
      ++decl_iter;
    }
    while (decl_iter != end && IsInstance<VarDecl>(*decl_iter)) {
      ++decl_iter;
    }
    while (decl_iter != end && IsInstance<FuncDef>(*decl_iter)) {
      ++decl_iter;
    }

    if (decl_iter != end) {
      auto decl = *decl_iter;
      EM.SyntaxError(decl->getLoc(), "unexpected", decl->GetClassName(),
                    Quote(decl->getName()));
    }

    // check the last declaration is the main function
    auto x = subclass_cast<FuncDef>(P->getDecls().back());
    if (!(x && x->getName() == "main")) {
      EM.SyntaxError(x->getLoc(), "expected main() at the end of input");
    }
  }

  void visitConstDecl(ConstDecl *CD) {
    if (CD->getType() == BasicTypeKind::Int &&
        !IsInstance<Num>(CD->getValue())) {
      EM.SyntaxError(CD->getLoc(), "const int", Quote(CD->getName()),
                    "expects an integer");
    } else if (CD->getType() == BasicTypeKind::Character &&
               !IsInstance<Char>(CD->getValue())) {
      EM.SyntaxError(CD->getLoc(), "const char", Quote(CD->getName()),
                    "expects a character");
    }
  }

  void visitVarDecl(VarDecl *VD) {
    if (VD->getType() == BasicTypeKind::Void) {
      EM.SyntaxError(VD->getLoc(), "cannot declare", Quote(VD->getName()),
                    "as a void variable");
    } else if (VD->getIsArray() && VD->getSize() == 0) {
      EM.SyntaxError(VD->getLoc(), "array size of", Quote(VD->getName()),
                    "cannot be 0");
    }
  }

  void visitArgDecl(ArgDecl *AD) {
    if (AD->getType() == BasicTypeKind::Void) {
      EM.SyntaxError(AD->getLoc(), "cannot declare void argument");
    }
  }

  void visitFuncDef(FuncDef *FD) {
    for (auto arg : FD->getArgs()) {
      VisitorBase::visitDecl(arg);
    }
    for (auto decl : FD->getDecls()) {
      VisitorBase::visitDecl(decl);
    }

    if (FD->getName() == "main") {
      if (FD->getReturnType() != BasicTypeKind::Void) {
        EM.SyntaxError(FD->getLoc(), "main() must return void");
      }
    }
  }

public:
  SyntaxChecker() : VisitorBase(), EM() {}
  bool Check(Program *P) {
    visitProgram(P);
    return EM.IsOk();
  }
};
} // namespace

bool simplecompiler::CheckSyntax(Program *P) {
  return SyntaxChecker().Check(P);
}
