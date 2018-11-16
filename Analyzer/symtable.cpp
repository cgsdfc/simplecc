#include "symtable.h"
#include "error.h"

Type *DeclToType(Decl *decl) {
  if (auto x = subclass_cast<ConstDecl>(decl))
    return DeclToType(x);
  if (auto x = subclass_cast<VarDecl>(decl))
    return DeclToType(x);
  return DeclToType(subclass_cast<FuncDef>(decl));
}

Type *DeclToType(ConstDecl *decl) {
  return new Constant(decl->type);
}

Type *DeclToType(VarDecl *decl) {
  auto  var_type = decl->type;
  if (var_type->is_array) {
    assert(var_type->size > 0);
    return new Array(var_type->type, var_type->size);
  }
  else {
    return new Variable(var_type->type);
  }
}

Type *DeclToType(FuncDef *decl) {
  auto fun = new Function(decl->return_type, {});
  for (auto arg: decl->args) {
    fun->args.push_back(arg->type);
  }
  return fun;
}

// Add one declaration to decl, checking redefinition
void DefineDecl(Decl* decl, Scope scope,
    TableType &dict, ErrorManager &e, const String &where) {

  const auto &name = GetDeclName(decl);
  if (dict.find(name) != dict.end()) {
    // key already there
    e.Error(decl->loc,
        "redefinition of identifier", Quote(name), "in", where);
  }
  else {
    auto type = DeclToType(decl);
    dict.emplace(std::make_pair(name,
          Entry(type, scope, decl->loc, name)));
  }
}

// Similar to DefineDecl(), but handle argument definition within
// a function
void DefineArg(Arg *arg,
    TableType &dict, ErrorManager &e, const String &funcname) {

  if (dict.find(arg->name) != dict.end()) {
    e.Error(arg->loc, "redefinition of argument", Quote(arg->name),
        "in function", Quote(funcname));
  }
  else {
    dict.emplace(std::make_pair(arg->name,
          Entry(new Variable(arg->type), Scope::Local,
            arg->loc, arg->name)));
  }
}

bool MakeGlobal(Program *prog, TableType &dict) {
  ErrorManager e;
  for (auto decl: prog->decls) {
    if (IsInstance<FuncDef>(decl))
      continue; // not now
    DefineDecl(decl, Scope::Global, dict, e, "<module>");
  }
  return e.IsOk();
}


class LocalResolver: public VisitorBase<LocalResolver> {
public:
    FuncDef *fun;
    const TableType &global;
    TableType &local;
    ErrorManager &e;

    LocalResolver(FuncDef *fun,
        const TableType &global, TableType &local, ErrorManager &e):
      fun(fun), global(global), local(local), e(e) {}

    void ResolveName(const String &name, const Location &loc) {
      if (local.find(name) != local.end())
        return;
      if (auto x = global.find(name); x != global.end()) {
        const auto &entry = x->second;
        local.emplace(std::make_pair(name,
              Entry(entry.type, entry.scope, loc, name)));
      }
      else {
        e.Error(loc,
            "undefined identifier", Quote(name), "in function", fun->name);
      }
    }

  void visit(Expr *expr) {
    VisitorBase::visit<void>(expr);
  }

  void visit(Stmt *stmt) {
    VisitorBase::visit<void>(stmt);
  }

  void visit(Read *x) {
    for (const auto &name: x->names)
      ResolveName(name, x->loc);
  }

  void visit(Write *x) {
    if (x->value)
      visit(x->value);
  }

  void visit(Assign *x) {
    visit(x->target);
    visit(x->value);
  }

  void visit(For *x) {
    visit(x->initial);
    visit(x->condition);
    visit(x->step);
    for (auto s: x->body)
      visit(s);
  }

  void visit(While *x) {
    visit(x->condition);
    for (auto s: x->body)
      visit(s);
  }

  void visit(Return *x) {
    if (x->value)
      visit(x->value);
  }

  void visit(If *x) {
    visit(x->test);
    for (auto s: x->body)
      visit(s);
    for (auto s: x->orelse)
      visit(s);
  }

  void visit(BinOp *x) {
    visit(x->left);
    visit(x->right);
  }

  void visit(UnaryOp *x) {
    visit(x->operand);
  }

  void visit(Call *x) {
    ResolveName(x->func, x->loc);
  }

  void visit(Num *x) {}
  void visit(Str *x) {}
  void visit(Char *x) {}

  void visit(Subscript *x) {
    ResolveName(x->name, x->loc);
  }

  void visit(Name *x) {
    ResolveName(x->id, x->loc);
  }

  void Resolve() {
    for (auto stmt: fun->stmts) {
      visit(stmt);
    }
  }
};

void MakeLocal(FuncDef *fun, TableType &top, TableType &local,
    ErrorManager &e) {
  // define fun in global first
  DefineDecl(fun, Scope::Global, top, e, "<module>");

  // define argument of a function
  for (auto arg: fun->args) {
    DefineArg(arg, local, e, fun->name);
  }

  // define const/var declarations of a function
  auto where = "function " + fun->name;
  for (auto decl: fun->decls) {
    DefineDecl(decl, Scope::Local, local, e, where);
  }

  // resolve local names
  LocalResolver resolver(fun, top, local, e);
  resolver.Resolve();
}

bool BuildSymbolTable(Program *prog, SymbolTable &table) {
  auto &global = table.global;
  if (!MakeGlobal(prog, global))
    return false;

  auto &locals = table.locals;
  ErrorManager e;

  for (auto decl: prog->decls) {
    if (auto fun = subclass_cast<FuncDef>(decl)) {
      TableType local;
      MakeLocal(fun, global, local, e);
      if (e.IsOk()) {
        locals.emplace(std::make_pair(fun->name, std::move(local)));
      }
    }
  }
  return e.IsOk();
}
