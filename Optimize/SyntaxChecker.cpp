#include "SyntaxChecker.h"
#include "ErrorManager.h"
#include "Visitor.h"

namespace {
using namespace simplecompiler;

class SyntaxChecker : private VisitorBase<SyntaxChecker> {
  ErrorManager e;
  friend class VisitorBase<SyntaxChecker>;

  void visitProgram(Program *node) {
    if (node->getDecls().size() == 0) {
      e.SyntaxError(Location(0, 0), "expected main() at the end of input");
      return;
    }
    for (auto decl : node->getDecls()) {
      VisitorBase::visitDecl<void>(decl);
    }

    auto decl_iter = node->getDecls().begin();
    auto end = node->getDecls().end();

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
      e.SyntaxError(decl->getLoc(), "unexpected", decl->GetClassName(),
                    Quote(decl->getName()));
    }

    // check the last declaration is the main function
    auto x = subclass_cast<FuncDef>(node->getDecls().back());
    if (!(x && x->getName() == "main")) {
      e.SyntaxError(x->getLoc(), "expected main() at the end of input");
    }
  }

  void visitConstDecl(ConstDecl *node) {
    if (node->getType() == BasicTypeKind::Int &&
        !IsInstance<Num>(node->getValue())) {
      e.SyntaxError(node->getLoc(), "const int", Quote(node->getName()),
                    "expects an integer");
    } else if (node->getType() == BasicTypeKind::Character &&
               !IsInstance<Char>(node->getValue())) {
      e.SyntaxError(node->getLoc(), "cont char", Quote(node->getName()),
                    "expects a character");
    }
  }

  void visitVarDecl(VarDecl *node) {
    if (node->getType() == BasicTypeKind::Void) {
      e.SyntaxError(node->getLoc(), "cannot declare", Quote(node->getName()),
                    "as a void variable");
    } else if (node->getIsArray() && node->getSize() == 0) {
      e.SyntaxError(node->getLoc(), "array size of", Quote(node->getName()),
                    "cannot be 0");
    }
  }

  void visitArgDecl(ArgDecl *node) {
    if (node->getType() == BasicTypeKind::Void) {
      e.SyntaxError(node->getLoc(), "cannot declare void argument");
    }
  }

  void visitFuncDef(FuncDef *node) {
    for (auto arg : node->getArgs()) {
      VisitorBase::visitDecl<void>(arg);
    }
    for (auto decl : node->getDecls()) {
      VisitorBase::visitDecl<void>(decl);
    }

    if (node->getName() == "main") {
      if (node->getReturnType() != BasicTypeKind::Void) {
        e.SyntaxError(node->getLoc(), "main() must return void");
      }
    }
  }

public:
  SyntaxChecker() : VisitorBase(), e() {}
  bool Check(Program *node) {
    visitProgram(node);
    return e.IsOk();
  }
};
} // namespace

bool simplecompiler::CheckSyntax(Program *node) {
  return SyntaxChecker().Check(node);
}
