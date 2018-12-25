#include "simplecc/CodeGen/Compile.h"
#include "simplecc/CodeGen/ByteCodeCompiler.h"
#include "simplecc/CodeGen/ByteCodePrinter.h"

namespace simplecc {
void PrintByteCode(Program *P, std::ostream &O) {
  ByteCodePrinter(O).PrintByteCode(P);
}

void CompileToByteCode(Program *P, const SymbolTable &S, ByteCodeModule &M) {
  ByteCodeCompiler().Compile(P, S, M);
}
} // namespace simplecc