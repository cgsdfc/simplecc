/// @file External interface of Target module.
#ifndef SIMPLECC_TARGET_TARGET_H
#define SIMPLECC_TARGET_TARGET_H
#include <iostream>

namespace simplecc {
class ByteCodeModule;

/// This function emits MIPS assembly from a ByteCodeModule.
/// Data will be written to M.
void AssembleMips(const ByteCodeModule &M, std::ostream &O);
} // namespace simplecc
#endif // SIMPLECC_TARGET_TARGET_H