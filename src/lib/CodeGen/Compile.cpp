#include "simplecc/CodeGen/Compile.h"
#include "simplecc/CodeGen/ByteCodeCompiler.h"
#include "simplecc/CodeGen/ByteCodePrinter.h"

namespace simplecc {
void PrintByteCode(ProgramAST *P, std::ostream &O) {
  ByteCodePrinter(O).PrintByteCode(P);
}

void CompileToByteCode(ProgramAST *P, const SymbolTable &S, ByteCodeModule &M) {
  ByteCodeCompiler().Compile(P, S, M);
}
} // namespace simplecc