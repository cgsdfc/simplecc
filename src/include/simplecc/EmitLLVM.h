#ifndef EMIT_LLVM_H
#define EMIT_LLVM_H
#include <string>

namespace simplecc {
using String = std::string;
class Program;
class SymbolTable;

bool CompileToLLVMIR(String InputFilename, Program *P, const SymbolTable &S,
                     String OutputFilename);
} // namespace simplecc

#endif
