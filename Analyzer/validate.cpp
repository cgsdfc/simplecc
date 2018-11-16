#include "validate.h"
#include "error.h"
#include "parser.h"

class SyntaxValidator {
public:

  bool visit(Program *node) {
    if (node->decls.size() == 0) {
      Error(Location(0, 0), "expected main() at the end of input");
      return false;
    }
    auto child_ok = visit(node->decls);
    auto decl_iter = node->decls.begin();
    auto end = node->decls.end();

    // check the order of declarations
    while (decl_iter != end && IsInstance<ConstDecl>(*decl_iter))
      ++decl_iter;
    while (decl_iter != end && IsInstance<VarDecl>(*decl_iter))
      ++decl_iter;
    while (decl_iter != end && IsInstance<FuncDef>(*decl_iter))
      ++decl_iter;

    if (decl_iter != end) {
      auto decl = *decl_iter;
      auto loc = decl->loc;
      Error(loc, "unexpected", decl->ClassName(), Quote(GetDeclName(decl)));
      return false;
    }

    // check the last declaration is the main function
    auto x = subclass_cast<FuncDef>(node->decls.back());
    if (!(x && x->name == "main")) {
      Error(x->loc, "expected main() at the end of input");
      return false;
    }

    return child_ok;
  }

  bool visit(ConstDecl *node) {
    if (node->type == BasicTypeKind::Int) {
      if (IsInstance<Num>(node->value)) {
        return true;
      }
      Error(node->loc, "const int", Quote(node->name), "expects an integer");
      return false;
    }
    else {
      assert(node->type == BasicTypeKind::Character);
      if (IsInstance<Char>(node->value)) {
        return true;
      }
      Error(node->loc, "cont char", Quote(node->name), "expects a character");
      return false;
    }
  }

  bool visit(Decl *node) {
    if (auto x = subclass_cast<ConstDecl>(node))
      return visit(x);
    else if (auto x = subclass_cast<VarDecl>(node))
      return visit(x);
    else {
      auto y = subclass_cast<FuncDef>(node);
      assert(y);
      return visit(y);
    }
  }

  bool visit(VarDecl *node) {
    auto var_type = node->type;
    if (var_type->type == BasicTypeKind::Void) {
      Error(node->loc, "cannot declare", Quote(node->name), "as a void variable");
      return false;
    }
    if (var_type->is_array && var_type->size == 0) {
      Error(node->loc, "array size of", Quote(node->name), "cannot be 0");
      return false;
    }
    return true;
  }

  bool visit(Arg *node, const String &funname) {
    if (node->type == BasicTypeKind::Void) {
      Error(node->loc, "cannot declare void argument", Quote(node->name),
          "of function", Quote(funname));
      return false;
    }
    return true;
  }

  template<typename T, typename... A>
    bool visit(const std::vector<T> &list, A&&... args) {
      bool ok = true;
      for (auto child: list) {
        if (!visit(child, args...)) {
          ok = false;
        }
      }
      return ok;
    }

  bool visit(FuncDef *node) {
    auto args_ok = visit(node->args, node->name);
    auto decls_ok = visit(node->decls);

    if (node->name == "main") {
      if (node->return_type != BasicTypeKind::Void) {
        Error(node->loc, "main() must return void");
        return false;
      }
    }

    return args_ok && decls_ok;
  }

};

bool ValidateSyntax(Program *node) {
  return SyntaxValidator().visit(node);
}
