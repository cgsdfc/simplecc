#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "AST.h"
#include "Types.h"

#include <cassert>
#include <iostream>
#include <unordered_map>

namespace simplecompiler {

using TableType = std::unordered_map<String, SymbolEntry>;

// Provide a safe const view to a sub-symbol table
class SymbolTableView {
  friend class SymbolTable;
  const TableType *subtable;
  explicit SymbolTableView(const TableType &subtable) : subtable(&subtable) {}

public:
  SymbolTableView() = default;
  SymbolTableView(const SymbolTableView &) = default;
  SymbolTableView(SymbolTableView &&) = default;
  ~SymbolTableView() = default;

  const SymbolEntry &operator[](const String &name) const {
    assert(subtable->count(name));
    return subtable->find(name)->second;
  }

  using const_iterator = TableType::const_iterator;
  const_iterator begin() const { return subtable->begin(); }
  const_iterator end() const { return subtable->end(); }
};

class SymbolTable {
public:
  /// Construct an empty SymbolTable
  SymbolTable() = default;

  void clear();

  /// Build itself from a program
  bool Build(Program *P);

  void SetExprType(Expr *E, BasicTypeKind T);

  // Return local symbol table for a function
  SymbolTableView GetLocal(FuncDef *FD) const;

  // Return a SymbolEntry for a global name
  SymbolEntry GetGlobal(const String &Name) const;

  // Return the BasicTypeKind for an expression
  BasicTypeKind GetExprType(Expr *E) const;

  void Format(std::ostream &O) const;

  TableType &getGlobal() { return GlobalTable; }
  TableType &getLocal(FuncDef *FD) { return LocalTables[FD]; }

private:
  TableType GlobalTable;
  std::unordered_map<FuncDef *, TableType> LocalTables;
  std::unordered_map<Expr *, BasicTypeKind> ExprTypes;
};

inline std::ostream &operator<<(std::ostream &os, const SymbolTable &t) {
  t.Format(os);
  return os;
}
} // namespace simplecompiler
#endif
