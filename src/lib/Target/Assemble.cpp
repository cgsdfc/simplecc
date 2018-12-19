#include "simplecc/Target/Assemble.h"
#include "simplecc/Target/MipsAssemblyWriter.h"

#include <iostream>

namespace simplecc {
void AssembleMips(const ByteCodeModule &M, std::ostream &O) {
  MipsAssemblyWriter().Write(M, O);
}

} // namespace simplecc
