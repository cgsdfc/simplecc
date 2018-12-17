#ifndef EMIT_LLVM_H
#define EMIT_LLVM_H
#include <string>

namespace simplecompiler {
using String = std::string;
class Program;
class SymbolTable;

bool CompileToLLVMIR(String InputFilename, Program *P, const SymbolTable &S,
                     String OutputFilename);
} // namespace simplecompiler

#endif
