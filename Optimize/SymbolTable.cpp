#include "SymbolTable.h"
#include "ErrorManager.h"
#include "Visitor.h"

#include <cassert>
#include <iostream>
#include <unordered_map>

namespace simplecompiler {

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
      EM.NameError(D->getLoc(), "redefinition of identifier", D->getName(),
                   "in function", TheFuncDef->getName());
      return;
    }
    if (TheGlobal->count(D->getName()) &&
        (*TheGlobal)[D->getName()].IsFunction()) {
      EM.NameError(D->getLoc(), "local identifier", D->getName(), "in function",
                   TheFuncDef->getName(), "shallows a global function");
      return;
    }
    /// Now we successfully define the name
    TheLocal->emplace(D->getName(), SymbolEntry(Scope::Local, D));
  }

  void DefineGlobalDecl(Decl *D) {
    assert(TheGlobal && "TheGlobal must be set!");
    if (TheGlobal->count(D->getName())) {
      EM.NameError(D->getLoc(), "redefinition of identifier", D->getName(),
                   "in <module>");
      return;
    }
    TheGlobal->emplace(D->getName(), SymbolEntry(Scope::Global, D));
  }

  void ResolveName(const String &Name, const Location &L) {
    assert(TheLocal && TheGlobal && TheFuncDef);
    if (TheLocal->count(Name))
      return;
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
  void visitName(Name *N) { ResolveName(N->getId(), N->getLoc()); }

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
      setFuncDef(static_cast<FuncDef *>(D));
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

  /// Trivial setters for important states during the construction
  /// of a table.
  void setFuncDef(FuncDef *FD) { TheFuncDef = FD; }
  void setTable(SymbolTable *ST) { TheTable = ST; }
  void setLocal(TableType *L) { TheLocal = L; }
  void setGlobal(TableType *G) { TheGlobal = G; }

  /// Clear the state of this SymbolTableBuilder
  void clear() {
    setTable(nullptr);
    setLocal(nullptr);
    setGlobal(nullptr);
    setFuncDef(nullptr);
    EM.clear();
  }

public:
  /// Default construct and destruct.
  SymbolTableBuilder() = default;
  ~SymbolTableBuilder() = default;

  /// Trivially copyable and moveable.
  SymbolTableBuilder(const SymbolTableBuilder &) = default;
  SymbolTableBuilder(SymbolTableBuilder &&) = default;

  /// Build a SymbolTable from a program.
  /// Return OK or not.
  /// Note: the table will be cleared first.
  bool Build(Program *P, SymbolTable &S) {
    clear();
    S.clear();
    setTable(&S);
    setGlobal(&S.getGlobal());
    visitProgram(P);
    return EM.IsOk();
  }
};

void SymbolTable::Format(std::ostream &O) const {
  O << "Global:\n";
  for (const std::pair<String, SymbolEntry> &Pair : GlobalTable) {
    O << "  " << Pair.first << ": " << Pair.second << "\n";
  }
  O << "\n";
  for (const std::pair<FuncDef *, TableType> &Pair : LocalTables) {
    O << "Local(" << Pair.first->getName() << "):\n";
    for (const std::pair<String, SymbolEntry> &Item : Pair.second) {
      O << "  " << Item.first << ": " << Item.second << "\n";
    }
    O << "\n";
  }
}

void SymbolTable::clear() {
  GlobalTable.clear();
  LocalTables.clear();
  ExprTypes.clear();
}

/// Return a SymbolTableView for a given FuncDef.
/// Assert on failure.
SymbolTableView SymbolTable::GetLocal(FuncDef *FD) const {
  assert(LocalTables.count(FD));
  return SymbolTableView(LocalTables.find(FD)->second);
}

// Return a SymbolEntry for a global name.
// Assert on failure.
SymbolEntry SymbolTable::GetGlobal(const String &Name) const {
  assert(GlobalTable.count(Name));
  return GlobalTable.find(Name)->second;
}

// Return the BasicTypeKind for an Expr.
// Assert on failure.
BasicTypeKind SymbolTable::GetExprType(Expr *E) const {
  assert(ExprTypes.count(E));
  return ExprTypes.find(E)->second;
}

void SymbolTable::SetExprType(Expr *E, BasicTypeKind T) {
  ExprTypes.emplace(E, T);
}

// public interface
bool SymbolTable::Build(Program *P) {
  return SymbolTableBuilder().Build(P, *this);
}
} // namespace simplecompiler
