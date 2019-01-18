/// @file External interface of the CodeGen module.
#ifndef SIMPLECC_CODEGEN_CODEGEN_H
#define SIMPLECC_CODEGEN_CODEGEN_H
#include <iostream>

namespace simplecc {
class ProgramAST;
class SymbolTable;
class ByteCodeModule;

/// PrintByteCode
void PrintByteCode(ProgramAST *P, std::ostream &O);
void CompileToByteCode(ProgramAST *P, const SymbolTable &S, ByteCodeModule &M);
} // namespace simplecc
#endif // SIMPLECC_CODEGEN_CODEGEN_H