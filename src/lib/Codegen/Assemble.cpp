#include "simplecc/Codegen/Assemble.h"
#include "simplecc/Codegen/MipsAssemblyWriter.h"

#include <iostream>

namespace simplecc {
void AssembleMips(const ByteCodeModule &M, std::ostream &O) {
  MipsAssemblyWriter().Write(M, O);
}

} // namespace simplecc
