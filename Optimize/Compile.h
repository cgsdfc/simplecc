#ifndef COMPILE_H
#define COMPILE_H
#include "ByteCode.h"
#include "SymbolTable.h"

#include <iostream>
#include <memory>
#include <utility>
#include <vector>

namespace simplecompiler {
class CompiledModule;
using SymbolEntryList = std::vector<SymbolEntry>;

class CompiledFunction {
  CompiledModule *Parent;
  SymbolTableView Symbols;
  std::vector<ByteCode> ByteCodeList;
  SymbolEntryList Arguments;
  SymbolEntryList LocalVariables;
  String Name;

  CompiledFunction(CompiledModule *M);

public:
  static CompiledFunction *Create(CompiledModule *M) {
    return new CompiledFunction(M);
  }

  const String &getName() const { return Name; }
  void setName(String Str) { Name = std::move(Str); }

  void Format(std::ostream &os) const;

  SymbolTableView GetLocal() const { return Symbols; }

  std::vector<ByteCode> &GetByteCodeList() { return ByteCodeList; }
  const std::vector<ByteCode> &GetByteCodeList() const { return ByteCodeList; }
  ByteCode &getByteCodeAt(unsigned Idx) { return ByteCodeList[Idx]; }
  const ByteCode &getByteCodeAt(unsigned Idx) const { return ByteCodeList[Idx]; }

  unsigned size() const { return ByteCodeList.size(); }
  bool empty() const { return ByteCodeList.empty(); }

  /// Iterator Interface.
  using iterator = decltype(ByteCodeList)::iterator;
  using const_iterator = decltype(ByteCodeList)::const_iterator;

  iterator begin() { return ByteCodeList.begin(); }
  iterator end() { return ByteCodeList.end(); }
  const_iterator begin() const { return ByteCodeList.begin(); }
  const_iterator end() const { return ByteCodeList.end(); }

  const SymbolEntryList &GetFormalArguments() const { return Arguments; }
  SymbolEntryList &GetFormalArguments() { return Arguments; }

  unsigned GetFormalArgumentCount() const { return Arguments.size(); }

  const SymbolEntryList &GetLocalObjects() const { return LocalVariables; }
  SymbolEntryList &GetLocalObjects() { return LocalVariables; }
};

class CompiledModule {
  using FunctionListTy = std::vector<CompiledFunction *>;
  FunctionListTy FunctionList;

  using StringLiteralTable = std::unordered_map<String, int>;
  StringLiteralTable StringLiterals;

  SymbolEntryList GlobalVariables;

public:
  CompiledModule() = default;
  void Build(Program *P, const SymbolTable &S);

  const FunctionListTy &getFunctionList() const { return FunctionList; }
  FunctionListTy &getFunctionList() { return FunctionList; }

  void Format(std::ostream &os) const;

  const StringLiteralTable &GetStringLiteralTable() const {
    return StringLiterals;
  }

  /// For a string literal, this method returns the corresponding ID.
  unsigned GetStringLiteralID(const String &Str);

  const SymbolEntryList &GetGlobalObjects() const { return GlobalVariables; }

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
  CompiledFunction *front() const { return FunctionList.front(); }
  CompiledFunction *back() const { return FunctionList.back(); }
};

inline std::ostream &operator<<(std::ostream &O, const CompiledFunction &c) {
  c.Format(O);
  return O;
}

inline std::ostream &operator<<(std::ostream &O, const CompiledModule &c) {
  c.Format(O);
  return O;
}
} // namespace simplecompiler
#endif
