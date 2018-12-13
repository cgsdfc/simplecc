#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "AST.h"
#include "Types.h"

#include <cassert>
#include <iostream>
#include <unordered_map>

namespace simplecompiler {

enum class Scope { Global, Local };
std::ostream &operator<<(std::ostream &os, Scope s);

// An entry in the SymbolTable with type and scope information about
// a name within a block (global or local).
class SymbolEntry {
  Scope scope;
  Decl *decl;

public:
  SymbolEntry(Scope scope, Decl *decl) : scope(scope), decl(decl) {}

  bool IsFunction() const;
  FuncType AsFunction() const;

  bool IsArray() const;
  ArrayType AsArray() const;

  bool IsVariable() const;
  VarType AsVariable() const;

  bool IsConstant() const;
  ConstType AsConstant() const;

  const char *GetTypeName() const;

  Location GetLocation() const;

  const String &GetName() const;

  Scope GetScope() const { return scope; }

  bool IsGlobal() const { return Scope::Global == GetScope(); }
  bool IsLocal() const { return Scope::Local == GetScope(); }

  bool IsFormalArgument() const;

  void Format(std::ostream &os) const;
};

inline std::ostream &operator<<(std::ostream &os, const SymbolEntry &e) {
  e.Format(os);
  return os;
}

using TableType = std::unordered_map<String, SymbolEntry>;
using StringLiteralTable = std::unordered_map<String, int>;

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

  // Sane operator[]
  const SymbolEntry &operator[](const String &name) const {
    assert(subtable->count(name));
    return subtable->find(name)->second;
  }

  TableType::const_iterator begin() const { return subtable->begin(); }

  TableType::const_iterator end() const { return subtable->end(); }
};

class SymbolTable {
  using NestedTableType = std::unordered_map<FuncDef *, TableType>;

  TableType global;
  NestedTableType locals;
  StringLiteralTable string_literals;
  std::unordered_map<Expr *, BasicTypeKind> expr_types;

public:
  /// Construct an empty SymbolTable
  SymbolTable() : global(), locals(), string_literals() {}

  /// Build itself from a program
  bool Build(Program *program);

  // Use only by TypeChecker
  void SetExprType(Expr *expr, BasicTypeKind type) {
    expr_types.emplace(expr, type);
  }

  // Return local symbol table for a function
  SymbolTableView GetLocal(FuncDef *fun) const {
    assert(locals.count(fun));
    return SymbolTableView(locals.find(fun)->second);
  }

  // Return a SymbolEntry for a global name
  SymbolEntry GetGlobal(const String &name) const {
    return SymbolTableView(global)[name];
  }

  // Return the ID for a string literal
  int GetStringLiteralID(const String &literal) const {
    assert(string_literals.count(literal));
    return string_literals.find(literal)->second;
  }

  // Return the string literal table
  const StringLiteralTable &GetStringLiteralTable() const {
    return string_literals;
  }

  // Return the BasicTypeKind for an expression
  BasicTypeKind GetExprType(Expr *expr) const {
    assert(expr_types.count(expr) && "Absent Expr Type");
    return expr_types.find(expr)->second;
  }

  // Self-check
  void Check() const;
  void Format(std::ostream &os) const;
};

inline std::ostream &operator<<(std::ostream &os, const SymbolTable &t) {
  t.Format(os);
  return os;
}
} // namespace simplecompiler
#endif
