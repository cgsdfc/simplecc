#ifndef EMIT_LLVM_H
#define EMIT_LLVM_H
#include <string>

namespace simplecc {
class Program;
class SymbolTable;

bool CompileToLLVMIR(std::string InputFile, Program *P, const SymbolTable &S,
                     std::string OutputFile);
} // namespace simplecc

#endif
