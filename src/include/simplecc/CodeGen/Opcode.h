#ifndef SIMPLECC_CODEGEN_OPCODE_H
#define SIMPLECC_CODEGEN_OPCODE_H
#include <iostream>

namespace simplecc {
enum class Opcode {
#define HANDLE_OPCODE(opcode, camelName) opcode,
#include "simplecc/CodeGen/Opcode.def"
};

const char *CStringFromOpcode(Opcode val);

inline std::ostream &operator<<(std::ostream &os, Opcode val) {
  return os << CStringFromOpcode(val);
}

} // namespace simplecc
#endif