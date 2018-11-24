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

  CompiledFunction(SymbolTableView local, SymbolEntry entry):
    local(local), code(), entry(entry) {
      assert(entry.IsFunction());
    }

  void Format(std::ostream &os) const;

  const std::vector<ByteCode> &GetCode() const {
    return code;
  }

  String GetName() const {
    return entry.GetName();
  }

};

class CompiledModule {
public:
  SymbolTableView global;
  std::vector<CompiledFunction*> functions;

  CompiledModule(SymbolTableView global): global(global) {}
  ~CompiledModule();

  const std::vector<CompiledFunction*> &GetFunctions() const {
    return functions;
  }

  void Format(std::ostream &os) const;
};

CompiledModule *CompileProgram(Program *prog, const SymbolTable &symtable);

inline std::ostream &operator<<(std::ostream &os, const CompiledFunction &c) {
  c.Format(os);
  return os;
}

inline std::ostream &operator<<(std::ostream &os, const CompiledModule &c) {
  c.Format(os);
  return os;
}

#endif
