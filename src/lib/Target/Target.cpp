#include "simplecc/Target/Target.h"
#include "simplecc/Target/MipsAssemblyWriter.h"

namespace simplecc {
void AssembleMips(const ByteCodeModule &M, std::ostream &O) {
  MipsAssemblyWriter().Write(M, O);
}

} // namespace simplecc
