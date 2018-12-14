#include "SymbolTable.h"
#include "ErrorManager.h"
#include "Visitor.h"

#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <utility> // move

namespace {
using namespace simplecompiler;

/// This all-in-one class does what MakeLocal(), MakeGlobal()
/// and LocalResolver do and in a uniform Visitor fashion.
class SymbolTableBuilder : ChildrenVisitor<SymbolTableBuilder> {
  /// Why Inherit from ChildrenVisitor:
  /// We inherit from ChildrenVisitor since SymbolTable does not
  /// concern about expression or statements. All it concerns is
  /// declaration and its use site -- Names. So we let ChildrenVisitor
  /// to automatically recurse into children while we only implement
  /// visitors of interest.
  friend class VisitorBase<SymbolTableBuilder>;
  friend class ChildrenVisitor<SymbolTableBuilder>;

  ErrorManager EM;
  TableType *TheGlobal;
  TableType *TheLocal;
  FuncDef *TheFuncDef;
  SymbolTable *TheTable;

  void DefineLocalDecl(Decl *D) {
    assert(TheLocal && "TheLocal must be set!");
    if (TheLocal->count(D->getName())) {
      EM.NameError(D->getLoc(), "redefinition of identifier",
          D->getName(), "in function", TheFuncDef->getName());
      return;
    }
    if (TheGlobal->count(D->getName()) &&
        (*TheGlobal)[D->getName()].IsFunction()) {
      EM.NameError(D->getLoc(), "local identifier", D->getName(),
          "in function", TheFuncDef->getName(), "shallows a global function");
      return;
    }
    /// Now we successfully define the name
    TheLocal->emplace(D->getName(), SymbolEntry(Scope::Local, D));
  }

  void DefineGlobalDecl(Decl *D) {
    assert(TheGlobal && "TheGlobal must be set!");
    if (TheGlobal->count(D->getName())) {
      EM.NameError(D->getLoc(), "redefinition of identifier",
          D->getName(), "in <module>");
      return;
    }
    TheGlobal->emplace(D->getName(), SymbolEntry(Scope::Global, D));
  }

  void ResolveName(const String &Name, const Location &L) {
    assert(TheLocal && TheGlobal && TheFuncDef);
    if (TheLocal->count(Name)) return;
    if (TheGlobal->count(Name)) {
      /// Fall back to globally.
      TheLocal->emplace(Name, (*TheGlobal)[Name]);
      return;
    }
    /// Undefined
    EM.NameError(L, "undefined identifier", Name, "in function",
        TheFuncDef->getName());
  }

  /// Overloads to visit AstNodes that have names.
  void visitName(Name *N) {
    ResolveName(N->getId(), N->getLoc());
  }

  void visitCall(Call *C) {
    ResolveName(C->getFunc(), C->getLoc());
    /// Recurse into children.
    ChildrenVisitor::visitCall(C);
  }

  void visitSubscript(Subscript *SB) {
    ResolveName(SB->getName(), SB->getLoc());
    /// Recurse into children.
    ChildrenVisitor::visitSubscript(SB);
  }

  // VisitorBase boilderplate.
  void visitStmt(Stmt *S) { return VisitorBase::visitStmt<void>(S); }
  void visitExpr(Expr *E) { return VisitorBase::visitExpr<void>(E); }

  void visitDecl(Decl *D) {
    switch (D->GetKind()) {
    case Decl::FuncDef:
      setFuncDef(static_cast<FuncDef*>(D));
      setLocal(&TheTable->getLocal(TheFuncDef));
      /// Define this function globally.
      DefineGlobalDecl(D);
      return visitFuncDef(TheFuncDef);

    case Decl::ConstDecl:
    case Decl::VarDecl:
    case Decl::ArgDecl:
      /* Fall through */
      return TheLocal ? DefineLocalDecl(D) : DefineGlobalDecl(D);
    default:
      EM.InternalError("Unhandled Decl subclass!");
    }
  }

  void setFuncDef(FuncDef *FD) { TheFuncDef = FD; }
  void setTable(SymbolTable *ST) { TheTable = ST; }
  void setLocal(TableType *L) { TheLocal = L; }
  void setGlobal(TableType *G) { TheGlobal = G; }

public:
  SymbolTableBuilder() = default;
  ~SymbolTableBuilder() = default;
  SymbolTableBuilder(const SymbolTableBuilder &) = default;
  SymbolTableBuilder(SymbolTableBuilder &&) = default;

  bool Build(Program *P, SymbolTable &S) {
    clear();
    setTable(&S);
    setGlobal(&S.getGlobal());
    visitProgram(P);
    return EM.IsOk();
  }

  /// Clear the state of this SymbolTableBuilder
  void clear() {
    setTable(nullptr);
    setLocal(nullptr);
    setGlobal(nullptr);
    setFuncDef(nullptr);
    EM.clear();
  }
};

// Define a declaration globally.
void DefineGlobalDecl(Decl *decl, TableType &global, ErrorManager &e) {
  if (global.count(decl->getName())) {
    e.NameError(decl->getLoc(), "redefinition of identifier",
                Quote(decl->getName()), "in <module>");
    return;
  }
  global.emplace(decl->getName(), SymbolEntry(Scope::Global, decl));
}

// Define a declaration locally.
// DeclLike takes Decl or Arg
void DefineLocalDecl(Decl *decl, TableType &local, const TableType &global,
                     ErrorManager &e, const String &funcname) {
  auto where = "in function " + Quote(funcname);
  if (local.count(decl->getName())) {
    e.NameError(decl->getLoc(), "redefinition of identifier",
                Quote(decl->getName()), where);
    return;
  }
  if (auto iter = global.find(decl->getName());
      iter != global.end() && iter->second.IsFunction()) {
    e.NameError(decl->getLoc(), "local identifier", Quote(decl->getName()),
                where, "shallows a global function name");
    return;
  }
  local.emplace(decl->getName(), SymbolEntry(Scope::Local, decl));
}

// Enter all global const/var declarations into dict
void MakeGlobal(Program *prog, TableType &dict, ErrorManager &e) {
  for (auto decl : prog->getDecls()) {
    if (IsInstance<FuncDef>(decl))
      continue; // not now
    DefineGlobalDecl(decl, dict, e);
  }
}

// Visitor that resolves local names for a function
class LocalResolver : ChildrenVisitor<LocalResolver> {
  friend class ChildrenVisitor<LocalResolver>;
  friend class VisitorBase<LocalResolver>;

  // The Ast of the function
  FuncDef *fun;
  // Global namespace to fail back
  const TableType &global;
  // Local namespace with local declarations and function arguments
  TableType &local;
  // Report errors
  ErrorManager &e;

  // Resolve one name
  void ResolveName(const String &name, const Location &loc) {
    if (local.find(name) != local.end())
      return; // already defined locally
    // defined globally
    if (auto x = global.find(name); x != global.end()) {
      local.emplace(name, x->second);
    } else {
      e.NameError(loc, "undefined identifier", Quote(name), "in function",
                  Quote(fun->getName()));
    }
  }

  // pull in VisitorBase's methods
  void visitStmt(Stmt *s) { return VisitorBase::visitStmt<void>(s); }

  void visitExpr(Expr *s) { return VisitorBase::visitExpr<void>(s); }

  void visitCall(Call *x) {
    ResolveName(x->getFunc(), x->getLoc());
    ChildrenVisitor::visitCall(x);
  }

  // these do not have identifiers
  void visitNum(Num *x) {}
  void visitStr(Str *x) {}
  void visitChar(Char *x) {}

  void visitSubscript(Subscript *x) {
    ResolveName(x->getName(), x->getLoc());
    ChildrenVisitor::visitSubscript(x);
  }

  void visitName(Name *x) { ResolveName(x->getId(), x->getLoc()); }

public:
  LocalResolver(FuncDef *fun, const TableType &global, TableType &local,
                ErrorManager &e)
      : fun(fun), global(global), local(local), e(e) {}

  // public interface
  void Resolve() {
    for (auto stmt : fun->getStmts()) {
      visitStmt(stmt);
    }
  }
};

// define and resolve names of a function
void MakeLocal(FuncDef *fun, TableType &top, TableType &local,
               ErrorManager &e) {
  // define fun itself in global first
  DefineGlobalDecl(fun, top, e);

  // define arguments of a function
  for (auto arg : fun->getArgs()) {
    DefineLocalDecl(arg, local, top, e, fun->getName());
  }

  // define const/var declarations of a function
  auto where = "function " + Quote(fun->getName());
  for (auto decl : fun->getDecls()) {
    DefineLocalDecl(decl, local, top, e, fun->getName());
  }

  // resolve local names
  LocalResolver resolver(fun, top, local, e);
  resolver.Resolve();
  // errors left in ErrorManager
}

void CheckTable(const TableType &table) {
  for (const auto &item : table) {
    assert(item.first == item.second.GetName());
  }
}

} // namespace

namespace simplecompiler {
// Overloads to print various data structures
std::ostream &operator<<(std::ostream &os, Scope s) {
  switch (s) {
  case Scope::Global:
    return os << "Global";
  case Scope::Local:
    return os << "Local";
  }
}

// Generic map printer
template <typename Key, typename Value>
std::ostream &operator<<(std::ostream &os,
                         const std::unordered_map<Key, Value> &t) {
  int i = 0;
  int size = t.size();

  os << "{";
  for (const auto &b : t) {
    os << b.first << ": " << b.second;
    if (i != size - 1) {
      os << ", ";
    }
    i++;
  }
  return os << "}";
}

// Specialized version for String key
template <typename Value>
std::ostream &operator<<(std::ostream &os,
                         const std::unordered_map<String, Value> &t) {
  int i = 0;
  int size = t.size();

  os << "{";
  for (const auto &b : t) {
    os << Quote(b.first) << ": " << b.second;
    if (i != size - 1) {
      os << ", ";
    }
    i++;
  }
  return os << "}";
}
} // namespace simplecompiler

void SymbolEntry::Format(std::ostream &os) const {
  os << "SymbolEntry("
   << GetName() << ", " << GetTypeName() << ", " << GetScope()
   << ", " << GetLocation() << ")";
}

void SymbolTable::Format(std::ostream &os) const {
  os << "Global:\n";
  for (const std::pair<String, SymbolEntry> &Pair : global) {
    os << "  " << Pair.first << ": " << Pair.second << "\n";
  }
  os << "\n";
  for (const std::pair<FuncDef*, TableType> &Pair : locals) {
    os << "Local(" << Pair.first->getName() << "):\n";
    for (const std::pair<String, SymbolEntry> &Item : Pair.second) {
      os << "  " << Item.first << ": " << Item.second << "\n";
    }
    os << "\n";
  }
}

// consistency check
void SymbolTable::Check() const {
  CheckTable(global);
  for (const auto &kv : locals) {
    CheckTable(kv.second);
    for (const auto &kv2 : kv.second) {
      assert(kv2.second.GetScope() == Scope::Local ||
             global.find(kv2.first) != global.end() &&
                 "entry in local with global scope must be present in global");
    }
  }
}

// public interface
bool SymbolTable::Build(Program *prog) {
  return SymbolTableBuilder().Build(prog, *this);
}
