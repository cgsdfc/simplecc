#ifndef SIMPLECC_CODEGEN_COMPILE_H
#define SIMPLECC_CODEGEN_COMPILE_H
#include <iostream>

namespace simplecc {
class Program;
class SymbolTable;
class ByteCodeModule;

void PrintByteCode(Program *P, std::ostream &O);
void CompileToByteCode(Program *P, const SymbolTable &S, ByteCodeModule &M);
} // namespace simplecc

#endif // SIMPLECC_CODEGEN_COMPILE_H
