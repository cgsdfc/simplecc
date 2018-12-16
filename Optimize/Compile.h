#ifndef COMPILE_H
#define COMPILE_H
#include "ByteCode.h"
#include "SymbolTable.h"

#include <iostream>
#include <memory>
#include <utility>
#include <vector>

namespace simplecompiler {
class ByteCodeModule;
using SymbolEntryList = std::vector<SymbolEntry>;

class ByteCodeFunction {
public:
  using ByteCodeListTy = std::vector<ByteCode>;

  static ByteCodeFunction *Create(ByteCodeModule *M) {
    return new ByteCodeFunction(M);
  }

  const String &getName() const { return Name; }
  void setName(String Str) { Name = std::move(Str); }

  void setLocalTable(SymbolTableView L) { Symbols = L; }
  SymbolTableView GetLocal() const { return Symbols; }

  ByteCodeListTy &GetByteCodeList() { return ByteCodeList; }
  const ByteCodeListTy &GetByteCodeList() const { return ByteCodeList; }

  ByteCode &getByteCodeAt(unsigned Idx) { return ByteCodeList[Idx]; }
  const ByteCode &getByteCodeAt(unsigned Idx) const {
    return ByteCodeList[Idx];
  }

  unsigned size() const { return ByteCodeList.size(); }
  bool empty() const { return ByteCodeList.empty(); }

  /// Iterator Interface.
  using iterator = ByteCodeListTy::iterator;
  using const_iterator = ByteCodeListTy::const_iterator;

  iterator begin() { return ByteCodeList.begin(); }
  iterator end() { return ByteCodeList.end(); }
  const_iterator begin() const { return ByteCodeList.begin(); }
  const_iterator end() const { return ByteCodeList.end(); }

  const SymbolEntryList &GetFormalArguments() const { return Arguments; }
  SymbolEntryList &GetFormalArguments() { return Arguments; }
  unsigned GetFormalArgumentCount() const { return Arguments.size(); }

  const SymbolEntryList &GetLocalVariables() const { return LocalVariables; }
  SymbolEntryList &GetLocalVariables() { return LocalVariables; }

  ByteCodeModule *getParent() const { return Parent; }
  void Format(std::ostream &os) const;

private:
  ByteCodeModule *Parent;
  SymbolTableView Symbols;
  ByteCodeListTy ByteCodeList;
  SymbolEntryList Arguments;
  SymbolEntryList LocalVariables;
  String Name;

  ByteCodeFunction(ByteCodeModule *M);

};

class ByteCodeModule {
public:
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
  unsigned GetStringLiteralID(const String &Str);

  /// Function and global variables interface.
  const FunctionListTy &getFunctionList() const { return FunctionList; }
  FunctionListTy &getFunctionList() { return FunctionList; }

  const SymbolEntryList &GetGlobalVariables() const { return GlobalVariables; }
  SymbolEntryList &GetGlobalVariables() { return GlobalVariables; }

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
  ByteCodeFunction *getFunctionAt(unsigned Idx) const { return FunctionList[Idx]; }
  /// Make this Module empty.
  void clear();

  void Format(std::ostream &os) const;

private:
  FunctionListTy FunctionList;
  StringLiteralTable StringLiterals;
  SymbolEntryList GlobalVariables;
};

inline std::ostream &operator<<(std::ostream &O, const ByteCodeFunction &c) {
  c.Format(O);
  return O;
}

inline std::ostream &operator<<(std::ostream &O, const ByteCodeModule &c) {
  c.Format(O);
  return O;
}
} // namespace simplecompiler
#endif
