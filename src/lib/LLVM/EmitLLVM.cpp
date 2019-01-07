#include "simplecc/LLVM/EmitLLVM.h"
#include "simplecc/LLVM/LLVMIRCompiler.h"

namespace simplecc {

/// Compile a program to LLVM IR, dump resultant code to stderr.
/// Return true for success.
bool CompileToLLVMIR(ProgramAST *P, const SymbolTable &S, llvm::raw_ostream &OS) {

  auto TheCompiler = llvm::make_unique<LLVMIRCompiler>(P, S);

  /// Compile to llvm::Module, fail fast.
  if (TheCompiler->Compile())
    return true;

  /// Write out the human-readable bitcode.
  TheCompiler->getModule().print(OS, nullptr);
  return false;
}

} // namespace simplecc
