#ifndef COMPILE_H
#define COMPILE_H
#include "symtable.h"
#include "code.h"

class CompiledFunction {
public:
  // Local symbols
  SymbolTableView local;
  // Compiled byte code
  std::vector<ByteCode> code;
  // Self identity
  SymbolEntry entry;
  // Number of formal arguments
  unsigned nargs;

  CompiledFunction(SymbolTableView local,
     const std::vector<ByteCode> &code, SymbolEntry entry):
    local(local), code(code), entry(entry) {
      assert(entry.IsFunction());
      nargs = entry.AsFunction().GetArgCount();
    }

  CompiledFunction(CompiledFunction &&other):
    local(other.local), code(std::move(other.code)), entry(other.entry) {}

  void Format(std::ostream &os) const;

  const std::vector<ByteCode> &GetCode() const {
    return code;
  }

  const String &GetName() const {
    return entry.GetName();
  }

  unsigned GetArgCount() const {
    return nargs;
  }
};

class CompiledModule {
public:
  SymbolTableView global;
  std::vector<CompiledFunction> functions;
  const StringLiteralTable &strings;

  CompiledModule(SymbolTableView global,
       std::vector<CompiledFunction> &&functions, const StringLiteralTable &strings):
    global(global),
    functions(std::move(functions)),
    strings(strings) {}

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
