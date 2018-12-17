#include "Assemble.h"
#include "MipsAssemblyWriter.h"

#include <iostream>

namespace simplecompiler {
void AssembleMips(const ByteCodeModule &M, std::ostream &O) {
  MipsAssemblyWriter().Write(M, O);
}

} // namespace simplecompiler
