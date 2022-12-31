// MIT License
// 
// Copyright (c) 2018 Cong Feng.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef SIMPLECC_ANALYSIS_SYMBOLTABLE_H
#define SIMPLECC_ANALYSIS_SYMBOLTABLE_H
#include "simplecc/Analysis/Types.h"
#include "simplecc/AST/AST.h"
#include <iostream>
#include <string>
#include <unordered_map>

namespace simplecc {
class SymbolTableBuilder;
using TableType = std::unordered_map<std::string, SymbolEntry>;

/// @brief LocalSymbolTable provides a readonly view to a local symbol table.
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

/// @brief SymbolTable holds all the symbols and expression of a program.
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

  void Format(std::ostream &O) const;

private:
  TableType GlobalTable;
  std::unordered_map<const FuncDef *, TableType> LocalTables;

  friend class SymbolTableBuilder;
  /// Return the global table to be populate.
  TableType &getGlobal() { return GlobalTable; }
  /// Create or Return a local table to be populate.
  TableType &getLocal(FuncDef *FD) { return LocalTables[FD]; }
};

DEFINE_INLINE_OUTPUT_OPERATOR(SymbolTable)
} // namespace simplecc
#endif // SIMPLECC_ANALYSIS_SYMBOLTABLE_H