#ifndef SIMPLECC_CODEGEN_BYTECODEFUNCTION_H
#define SIMPLECC_CODEGEN_BYTECODEFUNCTION_H
#include "simplecc/Analysis/SymbolTable.h"
#include "simplecc/Codegen/ByteCode.h"

#include <iostream>
#include <utility> // move()
#include <vector>

namespace simplecc {
class ByteCodeModule;

class ByteCodeFunction {
public:
  using LocalVariableListTy = std::vector<SymbolEntry>;
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

  const LocalVariableListTy &GetFormalArguments() const { return Arguments; }
  LocalVariableListTy &GetFormalArguments() { return Arguments; }
  unsigned GetFormalArgumentCount() const { return Arguments.size(); }

  /// Iterator of local variables.
  using local_iterator = LocalVariableListTy::iterator;
  using const_local_iterator = LocalVariableListTy::const_iterator;

  local_iterator local_begin() { return LocalVariables.begin(); }
  local_iterator local_end() { return LocalVariables.end(); }
  const_local_iterator local_begin() const { return LocalVariables.begin(); }
  const_local_iterator local_end() const { return LocalVariables.end(); }

  const LocalVariableListTy &GetLocalVariables() const {
    return LocalVariables;
  }
  LocalVariableListTy &GetLocalVariables() { return LocalVariables; }

  ByteCodeModule *getParent() const { return Parent; }
  void Format(std::ostream &O) const;

private:
  ByteCodeModule *Parent;
  SymbolTableView Symbols;
  ByteCodeListTy ByteCodeList;
  LocalVariableListTy Arguments;
  LocalVariableListTy LocalVariables;
  String Name;

  ByteCodeFunction(ByteCodeModule *M);
};

inline std::ostream &operator<<(std::ostream &O, const ByteCodeFunction &c) {
  c.Format(O);
  return O;
}
} // namespace simplecc

#endif
