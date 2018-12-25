#ifndef SIMPLECC_LLVM_EMITLLVM_H
#define SIMPLECC_LLVM_EMITLLVM_H
#include <llvm/Support/raw_ostream.h>
#include <string>

namespace simplecc {
class Program;
class SymbolTable;

bool CompileToLLVMIR(std::string InputFile, Program *P, const SymbolTable &S,
                     llvm::raw_ostream &OS);
} // namespace simplecc

#endif