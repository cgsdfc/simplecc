#ifndef SIMPLECOMPILER_COMPILE_H
#define SIMPLECOMPILER_COMPILE_H
#include <iostream>

namespace simplecc {
class Program;
class SymbolTable;
class ByteCodeModule;

void PrintByteCode(Program *P, std::ostream &O);
void CompileToByteCode(Program *P, const SymbolTable &S, ByteCodeModule &M);
} // namespace simplecc

#endif // SIMPLECOMPILER_COMPILE_H
