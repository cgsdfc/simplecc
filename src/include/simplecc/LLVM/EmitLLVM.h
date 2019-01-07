#ifndef SIMPLECC_LLVM_EMITLLVM_H
#define SIMPLECC_LLVM_EMITLLVM_H
#include <llvm/Support/raw_ostream.h>
#include <string>

namespace simplecc {
class ProgramAST;
class SymbolTable;

bool CompileToLLVMIR(ProgramAST *P, const SymbolTable &S, llvm::raw_ostream &OS);
} // namespace simplecc

#endif