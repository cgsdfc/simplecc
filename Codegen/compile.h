#ifndef COMPILE_H
#define COMPILE_H
#include "symtable.h"
#include "code.h"

class CompiledFunction {
public:
  SymbolTableView local;
  const SymbolTable &symtable;
  std::vector<ByteCode> code;
  CompiledFunction(SymbolTableView local, const SymbolTable &symtable):
    local(local), symtable(symtable), code() {}
};

class CompiledModule {
public:
  const SymbolTable &symtable;
  // All functions
  std::vector<CompiledFunction*> functions;

  CompiledModule(const SymbolTable &symtable): symtable(symtable) {}
};

CompiledModule *Compile(Program *prog, const SymbolTable &symtable);
#endif
