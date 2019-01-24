#include "simplecc/LLVM/LLVM.h"
#include "simplecc/LLVM/LLVMIRCompiler.h"

namespace simplecc {

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
