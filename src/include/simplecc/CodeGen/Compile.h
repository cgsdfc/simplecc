#ifndef SIMPLECC_CODEGEN_COMPILE_H
#define SIMPLECC_CODEGEN_COMPILE_H
#include <iostream>

namespace simplecc {
class ProgramAST;
class SymbolTable;
class ByteCodeModule;

void PrintByteCode(ProgramAST *P, std::ostream &O);
void CompileToByteCode(ProgramAST *P, const SymbolTable &S, ByteCodeModule &M);
} // namespace simplecc

#endif // SIMPLECC_CODEGEN_COMPILE_H
