#include "simplecc/LLVM/EmitLLVM.h"
#include "simplecc/LLVM/LLVMIRCompiler.h"

#include <llvm/Support/raw_ostream.h>
#include <system_error>
#include <memory>

namespace simplecc {

/// Compile a program to LLVM IR, dump resultant code to stderr.
/// Return true for success.
bool CompileToLLVMIR(String InputFilename, Program *P, const SymbolTable &S,
                     String OutputFilename) {
  auto TheCompiler = std::unique_ptr<LLVMIRCompiler>(LLVMIRCompiler::Create(InputFilename, P, S));

  /// Compile to llvm::Module, fail fast.
  bool OK = TheCompiler->Compile();
  if (!OK)
    return false;

  /// Try to open the output file, fail fast.
  std::error_code EC;
  /// raw_fd_ostream treat "-" specially as opening stdout.
  /// We honor its behavior.
  if (OutputFilename.empty())
    OutputFilename = "-";
  llvm::raw_fd_ostream OS(OutputFilename, EC);
  if (EC) {
    llvm::errs() << EC.message() << "\n";
    return false;
  }

  /// Write out the human-readable bitcode.
  TheCompiler->getModule().print(OS, nullptr);
  return true;
}

} // namespace simplecc
