#ifndef SIMPLECOMPILER_COMPILE_H
#define SIMPLECOMPILER_COMPILE_H

namespace simplecc {
class Program;
class SymbolTable;
class ByteCodeModule;

void Compile(Program *P, const SymbolTable &S, ByteCodeModule &Module);
}

#endif //SIMPLECOMPILER_COMPILE_H
