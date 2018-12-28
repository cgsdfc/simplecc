#include "simplecc/Optimize/Instruction.h"
#include <cassert>
#include <simplecc/Optimize/Instruction.h>

using namespace simplecc;

const char *Instruction::getOpcodeName(unsigned Opcode) {
  switch (Opcode) {
  case Ret:
    return "ret";
  case Br:
    return "br";
  case Neg:
    return "neg";
  case Add:
    return "add";
  case Sub:
    return "sub";
  case Mul:
    return "mul";
  case Div:
    return "div";
  case Shl:
    return "shl";
  case And:
    return "and";
  case Or:
    return "or";
  case Xor:
    return "xor";
  case Alloca:
    return "alloca";
  case Load:
    return "load";
  case Store:
    return "store";
  case GetElementPtr:
    return "getelementptr";
  case ICmp:
    return "icmp";
  case PHI:
    return "phi";
  case Call:
    return "call";
  default:
    return "<Invalid operator>";
  }
}

Instruction *Instruction::cloneImpl() const {
  assert(false && "subclass should implement cloneImpl()");
  return nullptr;
}
