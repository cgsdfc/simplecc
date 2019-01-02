#ifndef SIMPLECC_TARGET_ASSEMBLE_H
#define SIMPLECC_TARGET_ASSEMBLE_H
#include <iostream>

namespace simplecc {
class ByteCodeModule;
void AssembleMips(const ByteCodeModule &M, std::ostream &O);
} // namespace simplecc
#endif