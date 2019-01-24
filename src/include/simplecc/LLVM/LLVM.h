#ifndef SIMPLECC_LLVM_LLVM_H
#define SIMPLECC_LLVM_LLVM_H
#include <llvm/Support/raw_ostream.h>
#include <string>

namespace simplecc {
class ProgramAST;
class SymbolTable;

/// @brief Compile a program to LLVM IR, dump result to \param OS.
/// @return true if error happened.
bool CompileToLLVMIR(ProgramAST *P, const SymbolTable &S, llvm::raw_ostream &OS);
} // namespace simplecc
#endif // SIMPLECC_LLVM_LLVM_H