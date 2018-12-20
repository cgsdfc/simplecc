#ifndef EMIT_LLVM_H
#define EMIT_LLVM_H
#include <string>
#include <llvm/Support/raw_ostream.h>

namespace simplecc {
class Program;
class SymbolTable;

bool CompileToLLVMIR(std::string InputFile, Program *P, const SymbolTable &S,
                     llvm::raw_ostream &OS);
} // namespace simplecc

#endif