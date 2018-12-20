#ifndef BYTE_CODE_MODULE_H
#define BYTE_CODE_MODULE_H
#include "simplecc/Analysis/Types.h"

#include <iostream>
#include <unordered_map>
#include <vector>

namespace simplecc {
class Program;
class SymbolTable;
class ByteCodeFunction;

class ByteCodeModule {
public:
  using GlobalVariableListTy = std::vector<SymbolEntry>;
  using FunctionListTy = std::vector<ByteCodeFunction *>;
  using StringLiteralTable = std::unordered_map<String, int>;

  ByteCodeModule() = default;
  ~ByteCodeModule();

  /// Populate this Module with ByteCode.
  void Build(Program *P, const SymbolTable &S);

  /// String literal interface.
  const StringLiteralTable &GetStringLiteralTable() const {
    return StringLiterals;
  }
  /// For a string literal, this method returns the corresponding ID.
  unsigned getStringLiteralID(const String &Str);

  /// Function and global variables interface.
  const FunctionListTy &getFunctionList() const { return FunctionList; }
  FunctionListTy &getFunctionList() { return FunctionList; }

  const GlobalVariableListTy &getGlobalVariables() const {
    return GlobalVariables;
  }
  GlobalVariableListTy &getGlobalVariables() { return GlobalVariables; }

  /// Iterator Interface
  using iterator = FunctionListTy::iterator;
  using const_iterator = FunctionListTy::const_iterator;
  iterator begin() { return FunctionList.begin(); }
  iterator end() { return FunctionList.end(); }
  const_iterator begin() const { return FunctionList.begin(); }
  const_iterator end() const { return FunctionList.end(); }

  /// FunctionList Forwading Interface
  bool empty() const { return FunctionList.empty(); }
  unsigned size() const { return FunctionList.size(); }
  ByteCodeFunction *front() const { return FunctionList.front(); }
  ByteCodeFunction *back() const { return FunctionList.back(); }
  ByteCodeFunction *getFunctionAt(unsigned Idx) const {
    return FunctionList[Idx];
  }
  /// Make this Module empty.
  void clear();

  void Format(std::ostream &O) const;

private:
  FunctionListTy FunctionList;
  StringLiteralTable StringLiterals;
  GlobalVariableListTy GlobalVariables;
};

inline std::ostream &operator<<(std::ostream &O, const ByteCodeModule &c) {
  c.Format(O);
  return O;
}
} // namespace simplecc
#endif
