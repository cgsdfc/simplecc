#ifndef ASSEMBLE_H
#define ASSEMBLE_H
#include <iostream>

namespace simplecompiler {
class ByteCodeModule;
void AssembleMips(const ByteCodeModule &M, std::ostream &O);
} // namespace simplecompiler
#endif
