#ifndef COMPILE_H
#define COMPILE_H
#include "ByteCode.h"
#include "SymbolTable.h"

namespace simplecompiler {
using ObjectList = std::vector<SymbolEntry>;

class CompiledFunction {
  // Local symbols
  SymbolTableView local;
  // Compiled byte code
  std::vector<ByteCode> code;
  // Self identity
  SymbolEntry entry;
  // Formal arguments
  ObjectList formal_arguments;
  // Local non-constants
  ObjectList local_objects;

public:
  CompiledFunction(SymbolTableView local, std::vector<ByteCode> &&code,
                   SymbolEntry entry, ObjectList &&formal_arguments,
                   ObjectList &&local_objects);

  CompiledFunction(CompiledFunction &&other);

  void Format(std::ostream &os) const;

  SymbolTableView GetLocal() const { return local; }

  const std::vector<ByteCode> &GetCode() const { return code; }

  const String &GetName() const { return entry.GetName(); }

  const ObjectList &GetFormalArguments() const { return formal_arguments; }

  unsigned GetFormalArgumentCount() const { return formal_arguments.size(); }

  const ObjectList &GetLocalObjects() const { return local_objects; }
};

class CompiledModule {
  std::vector<CompiledFunction> functions;
  const StringLiteralTable &strings;
  ObjectList global_objects;

public:
  CompiledModule(std::vector<CompiledFunction> &&functions,
                 const StringLiteralTable &strings, ObjectList &&global_objects)
      : functions(std::move(functions)), strings(strings),
        global_objects(std::move(global_objects)) {}

  CompiledModule(CompiledModule &&other)
      : functions(std::move(other.functions)), strings(other.strings),
        global_objects(std::move(other.global_objects)) {}

  const std::vector<CompiledFunction> &GetFunctions() const {
    return functions;
  }

  void Format(std::ostream &os) const;

  const StringLiteralTable &GetStringLiteralTable() const { return strings; }

  const ObjectList &GetGlobalObjects() const { return global_objects; }
};

CompiledModule CompileProgram(Program *prog, const SymbolTable &symtable);

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
