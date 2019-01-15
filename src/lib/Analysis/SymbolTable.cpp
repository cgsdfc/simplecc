#include "simplecc/Analysis/SymbolTable.h"

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
  ExprTypeTable.clear();
}

LocalSymbolTable SymbolTable::getLocalTable(const FuncDef *FD) const {
  assert(LocalTables.count(FD));
  return LocalSymbolTable(LocalTables.find(FD)->second);
}

SymbolEntry SymbolTable::getGlobalEntry(const std::string &Name) const {
  assert(GlobalTable.count(Name));
  return GlobalTable.find(Name)->second;
}

BasicTypeKind SymbolTable::getExprType(const ExprAST *E) const {
  assert(ExprTypeTable.count(E));
  return ExprTypeTable.find(E)->second;
}

void SymbolTable::setExprType(const ExprAST *E, BasicTypeKind Ty) {
  ExprTypeTable.emplace(E, Ty);
}

SymbolEntry LocalSymbolTable::operator[](const std::string &Name) const {
  assert(TheTable->count(Name) && "Undefined Name");
  return TheTable->find(Name)->second;
}