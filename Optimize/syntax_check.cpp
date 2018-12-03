#include "syntax_check.h"
#include "Visitor.h"
#include "error.h"

namespace {
using namespace simplecompiler;
class SyntaxChecker : public VisitorBase<SyntaxChecker> {
  ErrorManager e;

public:
  SyntaxChecker() : VisitorBase(), e() {}

  void visitProgram(Program *node) {
    if (node->decls.size() == 0) {
      e.SyntaxError(Location(0, 0), "expected main() at the end of input");
      return;
    }
    for (auto decl : node->decls) {
      VisitorBase::visitDecl<void>(decl);
    }

    auto decl_iter = node->decls.begin();
    auto end = node->decls.end();

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
      e.SyntaxError(decl->loc, "unexpected", decl->GetClassName(),
                    Quote(decl->name));
    }

    // check the last declaration is the main function
    auto x = subclass_cast<FuncDef>(node->decls.back());
    if (!(x && x->name == "main")) {
      e.SyntaxError(x->loc, "expected main() at the end of input");
    }
  }

  void visitConstDecl(ConstDecl *node) {
    if (node->type == BasicTypeKind::Int && !IsInstance<Num>(node->value)) {
      e.SyntaxError(node->loc, "const int", Quote(node->name),
                    "expects an integer");
    } else if (node->type == BasicTypeKind::Character &&
               !IsInstance<Char>(node->value)) {
      e.SyntaxError(node->loc, "cont char", Quote(node->name),
                    "expects a character");
    }
  }

  void visitVarDecl(VarDecl *node) {
    if (node->type == BasicTypeKind::Void) {
      e.SyntaxError(node->loc, "cannot declare", Quote(node->name),
                    "as a void variable");
    } else if (node->is_array && node->size == 0) {
      e.SyntaxError(node->loc, "array size of", Quote(node->name),
                    "cannot be 0");
    }
  }

  void visitArg(Arg *node, const String &funname) {
    if (node->type == BasicTypeKind::Void) {
      e.SyntaxError(node->loc, "cannot declare void argument",
                    Quote(node->name), "of function", Quote(funname));
    }
  }

  void visitFuncDef(FuncDef *node) {
    for (auto arg : node->args) {
      visitArg(arg, node->name);
    }
    for (auto decl : node->decls) {
      VisitorBase::visitDecl<void>(decl);
    }

    if (node->name == "main") {
      if (node->return_type != BasicTypeKind::Void) {
        e.SyntaxError(node->loc, "main() must return void");
      }
    }
  }

  bool Check(Program *node) {
    visitProgram(node);
    return e.IsOk();
  }
};
} // namespace

bool simplecompiler::CheckSyntax(Program *node) {
  return SyntaxChecker().Check(node);
}
