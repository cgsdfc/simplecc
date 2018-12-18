#include "simplecc/SymbolTable.h"
#include "simplecc/SymbolTableBuilder.h"

#include <cassert>
#include <iostream>
#include <utility> // pair

using namespace simplecc;

void SymbolTable::Format(std::ostream &O) const {
  O << "Global:\n";
  for (const std::pair<const String, SymbolEntry> &Pair : GlobalTable) {
    O << "  " << Pair.first << ": " << Pair.second << "\n";
  }
  O << "\n";
  for (const std::pair<FuncDef *const, TableType> &Pair : LocalTables) {
    O << "Local(" << Pair.first->getName() << "):\n";
    for (const std::pair<const String, SymbolEntry> &Item : Pair.second) {
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
SymbolTableView SymbolTable::getLocalTable(FuncDef *FD) const {
  assert(LocalTables.count(FD));
  return SymbolTableView(LocalTables.find(FD)->second);
}

// Return a SymbolEntry for a global name.
// Assert on failure.
SymbolEntry SymbolTable::getGlobalEntry(const String &Name) const {
  assert(GlobalTable.count(Name));
  return GlobalTable.find(Name)->second;
}

// Return the BasicTypeKind for an Expr.
// Assert on failure.
BasicTypeKind SymbolTable::getExprType(Expr *E) const {
  assert(ExprTypes.count(E));
  return ExprTypes.find(E)->second;
}

void SymbolTable::setExprType(Expr *E, BasicTypeKind T) {
  ExprTypes.emplace(E, T);
}

// public interface
bool SymbolTable::Build(Program *P) {
  clear();
  return SymbolTableBuilder().Build(P, *this);
}

/// Return the SymbolEntry for a name.
/// Assert on failure.
const SymbolEntry &SymbolTableView::operator[](const String &Name) const {
  assert(TheTable->count(Name));
  return TheTable->find(Name)->second;
}
