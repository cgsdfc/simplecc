#ifndef COMPILE_H
#define COMPILE_H
#include "ByteCode.h"
#include "SymbolTable.h"

namespace simplecompiler {
using SymbolEntryList = std::vector<SymbolEntry>;

class CompiledFunction {
  // Local symbols
  SymbolTableView local;
  // Compiled byte code
  std::vector<ByteCode> code;
  // Self identity
  SymbolEntry entry;
  // Formal arguments
  SymbolEntryList formal_arguments;
  // Local non-constants
  SymbolEntryList local_objects;

public:
  CompiledFunction(SymbolTableView local, std::vector<ByteCode> code,
                   SymbolEntry entry, SymbolEntryList formal_arguments,
                   SymbolEntryList local_objects);

  CompiledFunction(CompiledFunction &&other);

  void Format(std::ostream &os) const;

  SymbolTableView GetLocal() const { return local; }

  const std::vector<ByteCode> &GetCode() const { return code; }

  const String &GetName() const { return entry.GetName(); }

  const SymbolEntryList &GetFormalArguments() const { return formal_arguments; }

  unsigned GetFormalArgumentCount() const { return formal_arguments.size(); }

  const SymbolEntryList &GetLocalObjects() const { return local_objects; }
};

class CompiledModule {
  std::vector<CompiledFunction> functions;
  StringLiteralTable strings;
  SymbolEntryList global_objects;

public:
  CompiledModule() = default;
  void Build(Program *P, const SymbolTable &S);

  const std::vector<CompiledFunction> &GetFunctions() const {
    return functions;
  }

  void Format(std::ostream &os) const;

  const StringLiteralTable &GetStringLiteralTable() const { return strings; }

  const SymbolEntryList &GetGlobalObjects() const { return global_objects; }
};

inline std::ostream &operator<<(std::ostream &os, const CompiledFunction &c) {
  c.Format(os);
  return os;
}

inline std::ostream &operator<<(std::ostream &os, const CompiledModule &c) {
  c.Format(os);
  return os;
}
} // namespace simplecompiler
#endif
