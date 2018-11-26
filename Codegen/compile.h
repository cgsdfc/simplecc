#ifndef COMPILE_H
#define COMPILE_H
#include "symtable.h"
#include "code.h"

class FunctionCompiler;
class ModuleCompiler;

class CompiledFunction {
  friend class FunctionCompiler;
  // Local symbols
  SymbolTableView local;
  // Compiled byte code
  std::vector<ByteCode> code;
  // Self identity
  SymbolEntry entry;
  // Formal arguments
  std::vector<SymbolEntry> formal_arguments;
  // Local non-constants
  std::vector<SymbolEntry> local_objects;

  CompiledFunction(SymbolTableView local,
     std::vector<ByteCode> &&code, SymbolEntry entry,
     std::vector<SymbolEntry> &&formal_arguments,
     std::vector<SymbolEntry> &&local_objects):
    local(local),
    code(std::move(code)),
    entry(entry),
    formal_arguments(std::move(formal_arguments)),
    local_objects(std::move(local_objects)) {
      assert(entry.IsFunction());
    }

public:
  CompiledFunction(CompiledFunction &&other):
    local(other.local), code(std::move(other.code)), entry(other.entry),
    formal_arguments(std::move(other.formal_arguments)),
    local_objects(std::move(other.local_objects)) {}

  void Format(std::ostream &os) const;

  const std::vector<ByteCode> &GetCode() const {
    return code;
  }

  const String &GetName() const {
    return entry.GetName();
  }

  const std::vector<SymbolEntry> &GetFormalArguments() const {
    return formal_arguments;
  }

  unsigned GetFormalArgumentCount() const {
    return formal_arguments.size();
  }

  const std::vector<SymbolEntry> &GetLocalObjects() const {
    return local_objects;
  }

};

class CompiledModule {
  friend class ModuleCompiler;
  SymbolTableView global;
  std::vector<CompiledFunction> functions;
  const StringLiteralTable &strings;

  CompiledModule(SymbolTableView global,
       std::vector<CompiledFunction> &&functions,
       const StringLiteralTable &strings):
    global(global),
    functions(std::move(functions)),
    strings(strings) {}

public:
  CompiledModule(CompiledModule &&other):
    global(other.global),
    functions(std::move(other.functions)),
    strings(other.strings) {}

  const std::vector<CompiledFunction> &GetFunctions() const {
    return functions;
  }

  SymbolTableView GetSymbols() const {
    return global;
  }

  void Format(std::ostream &os) const;

  const StringLiteralTable &GetStringLiteralTable() const {
    return strings;
  }

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

#endif
