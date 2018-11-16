#include "symtable.h"
#include "error.h"

Type *ConstDeclToType(ConstDecl *decl) {
  return new Constant(decl->type);
}

Type *VarDeclToType(VarDecl *decl) {
  auto  var_type = decl->type;
  if (var_type->is_array) {
    assert(var_type->size > 0);
    return new Array(var_type->type, var_type->size);
  }
  else {
    return new Variable(var_type->type);
  }
}

Type *FuncDefToType(FuncDef *decl) {
  auto fun = new Function(decl->return_type, {});
  for (auto arg: decl->args) {
    fun->args.push_back(arg->type);
  }
  return fun;
}

// convert a declaration to type
Type *DeclToType(Decl *decl) {
  if (auto x = subclass_cast<ConstDecl>(decl))
    return ConstDeclToType(x);
  if (auto x = subclass_cast<VarDecl>(decl))
    return VarDeclToType(x);
  return FuncDefToType(subclass_cast<FuncDef>(decl));
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

// Enter all global const/var declarations into dict
bool MakeGlobal(Program *prog, TableType &dict) {
  ErrorManager e;
  for (auto decl: prog->decls) {
    if (IsInstance<FuncDef>(decl))
      continue; // not now
    DefineDecl(decl, Scope::Global, dict, e, "<module>");
  }
  return e.IsOk();
}

// Visitor that resolves local names for a function
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
        return; // already defined locally
      // define globally
      if (auto x = global.find(name); x != global.end()) {
        // make a copy of the global entry
        Entry local_entry(x->second);
        local_entry.location = loc;
        local.emplace(std::make_pair(name, std::move(local_entry)));
      }
      else {
        e.Error(loc,
            "undefined identifier", Quote(name), "in function", fun->name);
      }
    }

  // visitor methods that handle each type of AST

  void visit(Expr *expr) {
    VisitorBase::visit<void>(expr);
  }

  void visit(Stmt *stmt) {
    VisitorBase::visit<void>(stmt);
  }

  void visitRead(Read *x) {
    for (const auto &name: x->names)
      ResolveName(name, x->loc);
  }

  void visitWrite(Write *x) {
    if (x->value)
      visit(x->value);
  }

  void visitAssign(Assign *x) {
    visit(x->target);
    visit(x->value);
  }

  void visitFor(For *x) {
    visit(x->initial);
    visit(x->condition);
    visit(x->step);
    for (auto s: x->body)
      visit(s);
  }

  void visitWhile(While *x) {
    visit(x->condition);
    for (auto s: x->body)
      visit(s);
  }

  void visitReturn(Return *x) {
    if (x->value)
      visit(x->value);
  }

  void visitExprStmt(ExprStmt *x) {
    return visit(x->value);
  }

  void visitIf(If *x) {
    visit(x->test);
    for (auto s: x->body)
      visit(s);
    for (auto s: x->orelse)
      visit(s);
  }

  void visitBinOp(BinOp *x) {
    visit(x->left);
    visit(x->right);
  }

  void visitUnaryOp(UnaryOp *x) {
    visit(x->operand);
  }

  void visitCall(Call *x) {
    ResolveName(x->func, x->loc);
  }

  void visitNum(Num *x) {}
  void visitStr(Str *x) {}
  void visitChar(Char *x) {}

  void visitSubscript(Subscript *x) {
    ResolveName(x->name, x->loc);
  }

  void visitName(Name *x) {
    ResolveName(x->id, x->loc);
  }

  // public interface
  void Resolve() {
    for (auto stmt: fun->stmts) {
      visit(stmt);
    }
  }
};

// define and resolve names for a function
void MakeLocal(FuncDef *fun,
    TableType &top, TableType &local, ErrorManager &e) {
  // define fun itself in global first
  DefineDecl(fun, Scope::Global, top, e, "<module>");

  // define arguments of a function
  for (auto arg: fun->args) {
    DefineArg(arg, local, e, fun->name);
  }

  // define const/var declarations of a function
  auto where = "function " + Quote(fun->name);
  for (auto decl: fun->decls) {
    DefineDecl(decl, Scope::Local, local, e, where);
  }

  // resolve local names
  LocalResolver resolver(fun, top, local, e);
  resolver.Resolve();
  // errors left in ErrorManager
}

bool BuildSymbolTable(Program *prog, SymbolTable &table) {
  // build global table first
  auto &global = table.global;
  if (!MakeGlobal(prog, global))
    return false;

  auto &locals = table.locals;
  ErrorManager e;

  // visit all FuncDef and build their local tables
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

std::ostream &operator<<(std::ostream &os, Scope s) {
  switch (s) {
  case Scope::Global:
    return os << "Scope::Global";
  case Scope::Local:
    return os << "Scope::Local";
  }
}

std::ostream &operator<<(std::ostream &os, const Entry& e) {
  os << "Entry(";
  os << "type=" << e.type->ClassName() << ", "
    << "scope=" << e.scope << ", "
    << "location=" << e.location << ", "
    << "name=" << e.name;
  return os << ")";
}

template <typename Value>
std::ostream &operator<<(
    std::ostream &os, const std::unordered_map<String, Value> &t) {
  os << "{";
  int i = 0;
  int size = t.size();
  for (const auto &b: t) {
    os << Quote(b.first) << ": " << b.second;
    if (i != size - 1) {
      os << ", ";
    }
    i++;
  }
  return os << "}";
}

std::ostream &operator<<(std::ostream &os, const SymbolTable &t) {
  os << "SymbolTable(";
  os << "global=" << t.global << ", "
    << "locals=" << t.locals;
  return os << ")";
}
