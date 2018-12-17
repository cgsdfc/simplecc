#include "ByteCode.h"
#include "ErrorManager.h"

#include <cassert>
#include <iomanip>
#include <iostream>

namespace simplecompiler {

ByteCode ByteCode::Create(Opcode Op, int Val) {
  ByteCode B(Op);
  B.SetIntOperand(Val);
  return B;
}

ByteCode ByteCode::Create(Opcode Op, const char *Val) {
  ByteCode B(Op);
  B.SetStrOperand(Val);
  return B;
}

ByteCode ByteCode::Create(Opcode Op, const char *Str, int Int) {
  ByteCode B(Op);
  B.SetStrOperand(Str);
  B.SetIntOperand(Int);
  return B;
}

bool ByteCode::IsJumpXXX(Opcode Op) {
  switch (Op) {
  case Opcode::JUMP_IF_TRUE:
  case Opcode::JUMP_IF_FALSE:
  case Opcode::JUMP_IF_NOT_EQUAL:
  case Opcode::JUMP_IF_EQUAL:
  case Opcode::JUMP_IF_GREATER:
  case Opcode::JUMP_IF_GREATER_EQUAL:
  case Opcode::JUMP_IF_LESS:
  case Opcode::JUMP_IF_LESS_EQUAL:
  case Opcode::JUMP_FORWARD:
    return true;
  default:
    return false;
  }
}

bool ByteCode::HasIntOperand(Opcode Op) {
  switch (Op) {
  case Opcode::JUMP_IF_TRUE:
  case Opcode::JUMP_IF_FALSE:
  case Opcode::JUMP_FORWARD:
  case Opcode::JUMP_IF_NOT_EQUAL:
  case Opcode::JUMP_IF_EQUAL:
  case Opcode::JUMP_IF_GREATER:
  case Opcode::JUMP_IF_GREATER_EQUAL:
  case Opcode::JUMP_IF_LESS:
  case Opcode::JUMP_IF_LESS_EQUAL:
  case Opcode::CALL_FUNCTION:
  case Opcode::LOAD_STRING:
  case Opcode::LOAD_CONST:
    return true;
  default:
    return false;
  }
}

bool ByteCode::HasStrOperand(Opcode Op) {
  switch (Op) {
  case Opcode::LOAD_LOCAL:
  case Opcode::LOAD_GLOBAL:
  case Opcode::STORE_LOCAL:
  case Opcode::STORE_GLOBAL:
  case Opcode::CALL_FUNCTION:
    return true;
  default:
    return false;
  }
}

bool ByteCode::HasNoOperand(Opcode Op) {
  switch (Op) {
  case Opcode::BINARY_ADD:
  case Opcode::BINARY_SUB:
  case Opcode::BINARY_MULTIPLY:
  case Opcode::BINARY_DIVIDE:
  case Opcode::UNARY_POSITIVE:
  case Opcode::UNARY_NEGATIVE:
  case Opcode::POP_TOP:
  case Opcode::PRINT_NEWLINE:
    return true;
  default:
    return false;
  }
}

void ByteCode::Format(std::ostream &O) const {
  /* O << std::setw(4) << GetSourceLineno(); */
  O << std::left << std::setw(4) << GetByteCodeOffset();
  O << std::left << std::setw(25) << GetOpcode();

  if (HasIntOperand()) {
    O << std::setw(10) << GetIntOperand();
  }

  if (HasStrOperand()) {
    O << std::setw(20) << GetStrOperand();
  }
}

} // namespace simplecompiler
