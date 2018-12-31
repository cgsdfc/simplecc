#include "simplecc/Analysis/SymbolTable.h"
#include "simplecc/Analysis/SymbolTableBuilder.h"

using namespace simplecc;

void SymbolTable::Format(std::ostream &O) const {
  O << "Global:\n";
  for (const auto &Pair : GlobalTable) {
    O << "  " << Pair.first << ": " << Pair.second << "\n";
  }
  O << "\n";
  for (const auto &Pair : LocalTables) {
    O << "Local(" << Pair.first->getName() << "):\n";
    for (const std::pair<const std::string, SymbolEntry> &Item : Pair.second) {
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
LocalSymbolTable SymbolTable::getLocalTable(FuncDef *FD) const {
  assert(LocalTables.count(FD));
  // TODO: Fix this const_cast.
  return LocalSymbolTable(const_cast<SymbolTable *>(this)->LocalTables.find(FD)->second);
}

// Return a SymbolEntry for a global name.
// Assert on failure.
SymbolEntry SymbolTable::getGlobalEntry(const std::string &Name) const {
  assert(GlobalTable.count(Name));
  return GlobalTable.find(Name)->second;
}

// Return the BasicTypeKind for an ExprAST.
// Assert on failure.
BasicTypeKind SymbolTable::getExprType(ExprAST *E) const {
  assert(ExprTypes.count(E));
  return ExprTypes.find(E)->second;
}

void SymbolTable::setExprType(ExprAST *E, BasicTypeKind Ty) {
  ExprTypes.emplace(E, Ty);
}

/// Return the SymbolEntry for a name.
/// Assert on failure.
const SymbolEntry &LocalSymbolTable::operator[](const std::string &Name) const {
  assert(TheTable->count(Name) && "Undefined Name");
  return TheTable->find(Name)->second;
}