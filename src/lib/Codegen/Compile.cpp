#include "simplecc/Codegen/Compile.h"
#include "simplecc/Codegen/ByteCodeCompiler.h"
#include "simplecc/Codegen/ByteCodePrinter.h"

namespace simplecc {
void PrintByteCode(Program *P, std::ostream &O) {
  ByteCodePrinter(O).PrintByteCode(P);
}

void CompileToByteCode(Program *P, const SymbolTable &S, ByteCodeModule &M) {
  ByteCodeCompiler().Compile(P, S, M);
}
}