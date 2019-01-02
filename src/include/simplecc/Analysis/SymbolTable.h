#ifndef SIMPLECC_ANALYSIS_SYMBOLTABLE_H
#define SIMPLECC_ANALYSIS_SYMBOLTABLE_H

#include "simplecc/Analysis/Types.h"
#include "simplecc/Parse/AST.h"

#include <iostream>
#include <string>
#include <unordered_map>

namespace simplecc {
class SymbolTableBuilder;
using TableType = std::unordered_map<std::string, SymbolEntry>;

// Provide a safe const view to a sub-symbol table
class LocalSymbolTable {
  friend class SymbolTable;
  TableType *TheTable;
  explicit LocalSymbolTable(TableType &T) : TheTable(&T) {}

public:
  LocalSymbolTable() = default;
  ~LocalSymbolTable() = default;

  LocalSymbolTable(const LocalSymbolTable &) = default;
  LocalSymbolTable(LocalSymbolTable &&) = default;

  LocalSymbolTable &operator=(const LocalSymbolTable &) = default;
  LocalSymbolTable &operator=(LocalSymbolTable &&) = default;

  const SymbolEntry &operator[](const std::string &Name) const;

  using const_iterator = TableType::const_iterator;
  const_iterator begin() const { return TheTable->begin(); }
  const_iterator end() const { return TheTable->end(); }
};

class SymbolTable {
public:
  /// Construct an empty SymbolTable
  SymbolTable() = default;

  void clear();

  void setExprType(ExprAST *E, BasicTypeKind Ty);

  // Return local symbol table for a function
  LocalSymbolTable getLocalTable(FuncDef *FD) const;

  // Return a SymbolEntry for a global name
  SymbolEntry getGlobalEntry(const std::string &Name) const;

  // Return the BasicTypeKind for an expression
  BasicTypeKind getExprType(ExprAST *E) const;

  void Format(std::ostream &O) const;

private:
  TableType GlobalTable;
  std::unordered_map<FuncDef *, TableType> LocalTables;
  std::unordered_map<ExprAST *, BasicTypeKind> ExprTypes;

  /// Interface for the builder.
  friend class SymbolTableBuilder;
  /// Return the global table to be populate.
  TableType &getGlobal() { return GlobalTable; }
  /// Create or Return a local table to be populate.
  TableType &getLocal(FuncDef *FD) { return LocalTables[FD]; }
};

inline std::ostream &operator<<(std::ostream &os, const SymbolTable &t) {
  t.Format(os);
  return os;
}
} // namespace simplecc
#endif