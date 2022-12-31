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

#ifndef SIMPLECC_CODEGEN_BYTECODEFUNCTION_H
#define SIMPLECC_CODEGEN_BYTECODEFUNCTION_H
#include "simplecc/Analysis/SymbolTable.h"
#include "simplecc/CodeGen/ByteCode.h"
#include <iostream>
#include <string>
#include <utility> // move()
#include <vector>

namespace simplecc {
class ByteCodeModule;

/// @brief ByteCodeFunction is a container of ByteCode's compiled from a user-defined function.
/// Basically it manages three lists:
/// 1. a list of formal arguments.
/// 2. a list of local variables (ArrayType and VarType).
/// 3. a list of ByteCode's.
class ByteCodeFunction {
public:
  /// The type for a list of local variables.
  using LocalVariableListTy = std::vector<SymbolEntry>;
  /// The type for a list of ByteCode's.
  using ByteCodeListTy = std::vector<ByteCode>;

  /// Default destructor. Element types for all lists have value semantic.
  ~ByteCodeFunction() = default;

  /// Create a ByteCodeFunction and insert it into a ByteCodeModule.
  static ByteCodeFunction *Create(ByteCodeModule *M) {
    return new ByteCodeFunction(M);
  }

  /// Return the name of the function.
  const std::string &getName() const { return Name; }

  /// Set the name of the function.
  void setName(std::string Str) { Name = std::move(Str); }

  /// Return the local symbol table.
  void setLocalTable(LocalSymbolTable L) { Symbols = L; }

  /// Set the local symbol table.
  LocalSymbolTable getLocalTable() const { return Symbols; }

  /// Return the list of ByteCode.
  ByteCodeListTy &getByteCodeList() { return ByteCodeList; }

  /// Return the list of ByteCode.
  const ByteCodeListTy &getByteCodeList() const { return ByteCodeList; }

  /// Return the ByteCode at the specific index.
  ByteCode &getByteCodeAt(unsigned Idx) { return ByteCodeList[Idx]; }

  /// Return the ByteCode at the specific index.
  const ByteCode &getByteCodeAt(unsigned Idx) const {
    return ByteCodeList[Idx];
  }

  /// Return the size of the ByteCode list.
  size_t size() const { return ByteCodeList.size(); }

  /// Return emptiness of the ByteCode list.
  bool empty() const { return ByteCodeList.empty(); }

  /// Iterator to the ByteCode list.
  using iterator = ByteCodeListTy::iterator;

  /// Const iterator to the ByteCode list.
  using const_iterator = ByteCodeListTy::const_iterator;

  /// Iterator boilerplate.
  iterator begin() { return ByteCodeList.begin(); }
  iterator end() { return ByteCodeList.end(); }
  const_iterator begin() const { return ByteCodeList.begin(); }
  const_iterator end() const { return ByteCodeList.end(); }

  /// Return the list of formal arguments.
  const LocalVariableListTy &getFormalArguments() const { return Arguments; }

  /// Return the list of formal arguments.
  LocalVariableListTy &getFormalArguments() { return Arguments; }

  /// Return the number of formal arguments.
  size_t getFormalArgumentCount() const { return Arguments.size(); }

  /// Iterator to the local variables.
  using local_iterator = LocalVariableListTy::iterator;

  /// Const iterator to the local variables.
  using const_local_iterator = LocalVariableListTy::const_iterator;

  /// Iterator boilerplate.
  local_iterator local_begin() { return LocalVariables.begin(); }
  local_iterator local_end() { return LocalVariables.end(); }
  const_local_iterator local_begin() const { return LocalVariables.begin(); }
  const_local_iterator local_end() const { return LocalVariables.end(); }

  /// Return the list of local variables.
  const LocalVariableListTy &getLocalVariables() const {
    return LocalVariables;
  }

  /// Return the list of local variables.
  LocalVariableListTy &getLocalVariables() { return LocalVariables; }

  /// Return the enclosing ByteCodeModule.
  ByteCodeModule *getParent() const { return Parent; }

  /// Format a ByteCodeFunction.
  void Format(std::ostream &O) const;

private:
  ByteCodeModule *Parent;
  LocalSymbolTable Symbols;
  ByteCodeListTy ByteCodeList;
  LocalVariableListTy Arguments;
  LocalVariableListTy LocalVariables;
  std::string Name;

  /// Private. Use Create() instead.
  explicit ByteCodeFunction(ByteCodeModule *M);
};

DEFINE_INLINE_OUTPUT_OPERATOR(ByteCodeFunction)

} // namespace simplecc
#endif // SIMPLECC_CODEGEN_BYTECODEFUNCTION_H