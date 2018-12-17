#include "simplecc/Assemble.h"
#include "simplecc/MipsAssemblyWriter.h"

#include <iostream>

namespace simplecc {
void AssembleMips(const ByteCodeModule &M, std::ostream &O) {
  MipsAssemblyWriter().Write(M, O);
}

} // namespace simplecc
