#include "validate.h"
#include "Visitor.h"
#include "error.h"

class SyntaxValidator: public VisitorBase<SyntaxValidator> {
  ErrorManager e;
public:

  SyntaxValidator(): VisitorBase(), e() {}

  void visitProgram(Program *node) {
    if (node->decls.size() == 0) {
      e.Error(Location(0, 0), "expected main() at the end of input");
      return;
    }
    for (auto decl: node->decls) {
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
      e.Error(decl->loc, "unexpected", decl->ClassName(), Quote(GetDeclName(decl)));
    }

    // check the last declaration is the main function
    auto x = subclass_cast<FuncDef>(node->decls.back());
    if (!(x && x->name == "main")) {
      e.Error(x->loc, "expected main() at the end of input");
    }
  }

  void visitConstDecl(ConstDecl *node) {
    if (node->type == BasicTypeKind::Int) {
      if (!IsInstance<Num>(node->value)) {
        e.Error(node->loc, "const int", Quote(node->name), "expects an integer");
      }
    }
    else {
      assert(node->type == BasicTypeKind::Character);
      if (!IsInstance<Char>(node->value)) {
        e.Error(node->loc, "cont char", Quote(node->name), "expects a character");
      }
    }
  }

  void visitVarDecl(VarDecl *node) {
    if (node->type == BasicTypeKind::Void) {
      e.Error(node->loc, "cannot declare", Quote(node->name), "as a void variable");
    }
    else if (node->is_array && node->size == 0) {
      e.Error(node->loc, "array size of", Quote(node->name), "cannot be 0");
    }
  }

  void visitArg(Arg *node, const String &funname) {
    if (node->type == BasicTypeKind::Void) {
      e.Error(node->loc, "cannot declare void argument", Quote(node->name),
          "of function", Quote(funname));
    }
  }

  void visitFuncDef(FuncDef *node) {
    for (auto arg: node->args) {
      visitArg(arg, node->name);
    }
    for (auto decl: node->decls) {
      VisitorBase::visitDecl<void>(decl);
    }

    if (node->name == "main") {
      if (node->return_type != BasicTypeKind::Void) {
        e.Error(node->loc, "main() must return void");
      }
    }
  }

  bool Validate(Program *node) {
    visitProgram(node);
    return e.IsOk();
  }

};

bool ValidateSyntax(Program *node) {
  return SyntaxValidator().Validate(node);
}
