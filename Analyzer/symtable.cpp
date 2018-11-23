#include "symtable.h"
#include "Visitor.h"
#include "error.h"

inline Type *ConstDeclToType(ConstDecl *decl) {
  return new Constant(decl->type);
}

Type *VarDeclToType(VarDecl *decl) {
  if (decl->is_array) {
    assert(decl->size > 0);
    return new Array(decl->type, decl->size);
  }
  else {
    return new Variable(decl->type);
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

inline Type *DeclToType(Arg *arg) {
  return new Variable(arg->type);
}

// Define a declaration globally.
void DefineGlobalDecl(Decl *decl, TableType &global, ErrorManager &e) {
  if (global.count(decl->name)) {
    e.NameError(decl->loc, "redefinition of identifier", Quote(decl->name), "in <module>");
    return;
  }
  global.emplace(std::make_pair(decl->name, Entry(DeclToType(decl), Scope::Global, decl->loc, decl->name)));
}

// Define a declaration locally.
// DeclLike takes Decl or Arg
template <typename DeclLike>
void DefineLocalDecl(DeclLike *decl, TableType &local, const TableType &global,
                     ErrorManager &e, const String &funcname) {
  auto where = "in function " + Quote(funcname);
  if (local.count(decl->name)) {
    e.NameError(decl->loc,
        "redefinition of identifier", Quote(decl->name), where);
    return;
  }
  if (auto iter = global.find(decl->name); iter != global.end() && IsInstance<Function>(iter->second.type)) {
    e.NameError(decl->loc,
        "local identifier", Quote(decl->name), where, "shallows a global function name");
    return;
  }
  local.emplace(std::make_pair(decl->name, Entry(DeclToType(decl), Scope::Local, decl->loc, decl->name)));
}

// Enter all global const/var declarations into dict
void MakeGlobal(Program *prog, TableType &dict, ErrorManager &e) {
  for (auto decl: prog->decls) {
    if (IsInstance<FuncDef>(decl))
      continue; // not now
    DefineGlobalDecl(decl, dict, e);
  }
}

// Visitor that resolves local names for a function
class LocalResolver: public VisitorBase<LocalResolver>,
                     public ChildrenVisitor<LocalResolver> {
public:
  // The Ast of the function
  FuncDef *fun;
  // Global namespace to fail back
  const TableType &global;
  // Local namespace with local declarations and function arguments
  TableType &local;
  // Report errors
  ErrorManager &e;

  LocalResolver(FuncDef *fun,
      const TableType &global, TableType &local, ErrorManager &e):
    fun(fun), global(global), local(local), e(e) {}

  // Resolve one name
  void ResolveName(const String &name, const Location &loc) {
    if (local.find(name) != local.end())
      return; // already defined locally
    // define globally
    if (auto x = global.find(name); x != global.end()) {
      // make a copy of the global entry
      Entry local_entry(x->second);
      local_entry.location = loc; // use local location
      local.emplace(std::make_pair(name, std::move(local_entry)));
    }
    else {
      e.NameError(loc,
          "undefined identifier", Quote(name), "in function", Quote(fun->name));
    }
  }

  // pull in VisitorBase's methods
  void visitStmt(Stmt *s) {
    return VisitorBase::visitStmt<void>(s);
  }

  void visitExpr(Expr *s) {
    return VisitorBase::visitExpr<void>(s);
  }

  void visitCall(Call *x) {
    ResolveName(x->func, x->loc);
    ChildrenVisitor::visitCall(x);
  }

  // these do not have identifiers
  void visitNum(Num *x) {}
  void visitStr(Str *x) {}
  void visitChar(Char *x) {}

  void visitSubscript(Subscript *x) {
    ResolveName(x->name, x->loc);
    ChildrenVisitor::visitSubscript(x);
  }

  void visitName(Name *x) {
    ResolveName(x->id, x->loc);
  }

  // public interface
  void Resolve() {
    for (auto stmt: fun->stmts) {
      visitStmt(stmt);
    }
  }
};

// define and resolve names of a function
void MakeLocal(FuncDef *fun,
    TableType &top, TableType &local, ErrorManager &e) {
  // define fun itself in global first
  DefineGlobalDecl(fun, top, e);

  // define arguments of a function
  for (auto arg: fun->args) {
    DefineLocalDecl(arg, local, top, e, fun->name);
  }

  // define const/var declarations of a function
  auto where = "function " + Quote(fun->name);
  for (auto decl: fun->decls) {
    DefineLocalDecl(decl, local, top, e, fun->name);
  }

  // resolve local names
  LocalResolver resolver(fun, top, local, e);
  resolver.Resolve();
  // errors left in ErrorManager
}

// public interface
bool BuildSymbolTable(Program *prog, SymbolTable &table) {
  ErrorManager e;
  // build global table first
  auto &global = table.global;
  // no early return to check more errors
  MakeGlobal(prog, global, e);

  // visit all FuncDef and build their local tables
  auto &locals = table.locals;
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

void CheckTable(const TableType &table) {
  for (const auto &item: table) {
    assert(item.first == item.second.name);
    assert(item.second.type);
  }
}

// consistency check
void SymbolTable::Check() const {
  CheckTable(global);
  for (const auto &kv: locals) {
    CheckTable(kv.second);
    for (const auto &kv2: kv.second) {
      assert(kv2.second.scope == Scope::Local ||
          global.find(kv2.first) != global.end() &&
          "entry in local with global scope must be present in global");
    }
  }
}

const Entry &SymbolTable::LookupLocal(const String &fun, const String &name) const {
  auto iter = locals.find(fun);
  assert(iter != locals.end() && "absent function name in locals");
  const auto &local = iter->second;
  const auto &entry_iter = local.find(name);
  assert(entry_iter != local.end() && "absent name in local");
  return entry_iter->second;
}

const Entry &SymbolTable::LookupGlobal(const String &name) const {
  auto iter = global.find(name);
  assert(iter != global.end() && "absent name in global");
  return iter->second;
}

// Overloads to print various data structures
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

// print a dict with string key and generic value
template <typename Value>
std::ostream &operator<<(
    std::ostream &os, const std::unordered_map<String, Value> &t) {
  int i = 0;
  int size = t.size();

  os << "{";
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
