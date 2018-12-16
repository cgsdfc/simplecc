#ifndef ASSEMBLE_H
#define ASSEMBLE_H
#include <iostream>

namespace simplecompiler {
class ByteCodeModule;
void AssembleMips(const ByteCodeModule &module, std::ostream &os);
} // namespace simplecompiler
#endif
