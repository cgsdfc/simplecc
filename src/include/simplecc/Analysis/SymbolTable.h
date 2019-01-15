#ifndef SIMPLECC_ANALYSIS_SYMBOLTABLE_H
#define SIMPLECC_ANALYSIS_SYMBOLTABLE_H
#include "simplecc/Analysis/Types.h"
#include "simplecc/Parse/AST.h"
#include <iostream>
#include <string>
#include <unordered_map>

namespace simplecc {
class SymbolTableBuilder;
class TypeChecker;
using TableType = std::unordered_map<std::string, SymbolEntry>;

/// LocalSymbolTable provides a readonly view to a local symbol table.
/// It overloads the ``operator[]`` to provide readonly access to SymbolEntry
/// and prevents lookup failure by assertion.
class LocalSymbolTable {
  friend class SymbolTable;
  const TableType *TheTable;
  explicit LocalSymbolTable(const TableType &T) : TheTable(&T) {}

public:
  /// Define trivial constructor and destructor.
  LocalSymbolTable() = default;
  ~LocalSymbolTable() = default;

  /// Define trivial copy operations.
  LocalSymbolTable(const LocalSymbolTable &) = default;
  LocalSymbolTable &operator=(const LocalSymbolTable &) = default;

  /// Return the SymbolEntry for a name.
  SymbolEntry operator[](const std::string &Name) const;

  /// Readonly iterator interface.
  using const_iterator = TableType::const_iterator;
  const_iterator begin() const { return TheTable->begin(); }
  const_iterator end() const { return TheTable->end(); }
};

/// SymbolTable holds all the symbols and expression of a program.
class SymbolTable {
public:
  /// Construct an empty SymbolTable.
  SymbolTable() = default;
  /// Disable copying.
  SymbolTable(const SymbolTable &) = delete;
  /// Clear the table.
  void clear();

  /// Return LocalSymbolTable for a function.
  LocalSymbolTable getLocalTable(const FuncDef *FD) const;

  /// Return a SymbolEntry for a global name.
  SymbolEntry getGlobalEntry(const std::string &Name) const;

  /// Return the type of an expression.
  BasicTypeKind getExprType(const ExprAST *E) const;

  void Format(std::ostream &O) const;

private:
  TableType GlobalTable;
  std::unordered_map<const FuncDef *, TableType> LocalTables;
  std::unordered_map<const ExprAST *, BasicTypeKind> ExprTypeTable;

  friend class SymbolTableBuilder;
  /// Return the global table to be populate.
  TableType &getGlobal() { return GlobalTable; }
  /// Create or Return a local table to be populate.
  TableType &getLocal(FuncDef *FD) { return LocalTables[FD]; }

  // TODO: eliminate the ExprTypeTable.
  friend class TypeChecker;
  /// For TypeChecker to set type of an expression.
  void setExprType(const ExprAST *E, BasicTypeKind Ty);
};

DEFINE_INLINE_OUTPUT_OPERATOR(SymbolTable)
} // namespace simplecc
#endif // SIMPLECC_ANALYSIS_SYMBOLTABLE_H