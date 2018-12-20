#include "simplecc/LLVM/EmitLLVM.h"
#include "simplecc/LLVM/LLVMIRCompiler.h"

#include <llvm/Support/raw_ostream.h>
#include <system_error>
#include <memory>

namespace simplecc {

/// Compile a program to LLVM IR, dump resultant code to stderr.
/// Return true for success.
bool CompileToLLVMIR(std::string InputFile, Program *P, const SymbolTable &S,
                     std::string OutputFile) {

  std::unique_ptr<LLVMIRCompiler> TheCompiler(new LLVMIRCompiler(std::move(InputFile), P, S));

  /// Compile to llvm::Module, fail fast.
  bool OK = TheCompiler->Compile();
  if (!OK)
    return false;

  /// Try to open the output file, fail fast.
  std::error_code EC;
  llvm::raw_fd_ostream OS(OutputFile, EC);
  if (EC) {
    llvm::errs() << EC.message() << "\n";
    return false;
  }

  /// Write out the human-readable bitcode.
  TheCompiler->getModule().print(OS, nullptr);
  return true;
}

} // namespace simplecc
