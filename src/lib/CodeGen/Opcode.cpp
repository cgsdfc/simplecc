#include "simplecc/CodeGen/Opcode.h"
#include <cassert>

namespace simplecc {
const char *CStringFromOpcode(Opcode val) {
  switch (val) {
  default: assert(false && "Invalid Opcode");
#define HANDLE_OPCODE(opcode) case Opcode::opcode: return #opcode;
#include "simplecc/CodeGen/Opcode.def"
  }
}
} // namespace simplecc