#ifndef EMIT_LLVM_H
#define EMIT_LLVM_H

namespace simplecompiler {
class Program;
class SymbolTable;

bool CompileToLLVMIR(Program *P, const SymbolTable &S);
} // namespace simplecompiler

#endif
