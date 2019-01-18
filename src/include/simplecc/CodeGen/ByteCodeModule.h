#ifndef SIMPLECC_CODEGEN_BYTECODEMODULE_H
#define SIMPLECC_CODEGEN_BYTECODEMODULE_H
#include "simplecc/Analysis/Types.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace simplecc {
class ProgramAST;
class SymbolTable;
class ByteCodeFunction;

/// @brief ByteCodeModule is a container of ByteCodeFunction.
class ByteCodeModule {
public:
  /// A list of global variables.
  using GlobalVariableListTy = std::vector<SymbolEntry>;
  /// A list of global functions.
  using FunctionListTy = std::vector<ByteCodeFunction *>;
  /// The string literal unique pool.
  using StringLiteralTable = std::unordered_map<std::string, unsigned>;

  /// Construct an empty module.
  ByteCodeModule() = default;
  /// Destruct all owned ByteCodeFunction.
  ~ByteCodeModule();

  /// String literal interface.
  const StringLiteralTable &getStringLiteralTable() const {
    return StringLiterals;
  }
  /// For a string literal, this method returns the corresponding ID.
  unsigned getStringLiteralID(const std::string &Str);

  /// Function and global variables interface.
  const FunctionListTy &getFunctionList() const { return FunctionList; }
  FunctionListTy &getFunctionList() { return FunctionList; }

  const GlobalVariableListTy &getGlobalVariables() const {
    return GlobalVariables;
  }
  GlobalVariableListTy &getGlobalVariables() { return GlobalVariables; }

  /// Iterator to the function list.
  using iterator = FunctionListTy::iterator;
  using const_iterator = FunctionListTy::const_iterator;
  iterator begin() { return FunctionList.begin(); }
  iterator end() { return FunctionList.end(); }
  const_iterator begin() const { return FunctionList.begin(); }
  const_iterator end() const { return FunctionList.end(); }

  /// FunctionList Forwarding Interface
  bool empty() const { return FunctionList.empty(); }
  size_t size() const { return FunctionList.size(); }

  /// Make this module empty.
  void clear();
  void Format(std::ostream &O) const;

private:
  FunctionListTy FunctionList;
  StringLiteralTable StringLiterals;
  GlobalVariableListTy GlobalVariables;
};

DEFINE_INLINE_OUTPUT_OPERATOR(ByteCodeModule)

} // namespace simplecc
#endif // SIMPLECC_CODEGEN_BYTECODEMODULE_H