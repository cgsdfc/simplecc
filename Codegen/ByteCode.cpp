#include "ByteCode.h"
#include "error.h"

bool IsJumpXXX(Opcode op) {
  switch (op) {
  case Opcode::JUMP_IF_TRUE:
  case Opcode::JUMP_IF_FALSE:
  case Opcode::JUMP_FORWARD:
  case Opcode::JUMP_IF_NOT_EQUAL:
  case Opcode::JUMP_IF_EQUAL:
  case Opcode::JUMP_IF_GREATER:
  case Opcode::JUMP_IF_GREATER_EQUAL:
  case Opcode::JUMP_IF_LESS:
  case Opcode::JUMP_IF_LESS_EQUAL:
    return true;
  default:
    return false;
  }
}

bool HasIntOperand(Opcode op) {
  switch (op) {
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
    return true;
  default:
    return false;
  }
}

bool HasStrOperand(Opcode op) {
  switch (op) {
  case Opcode::LOAD_LOCAL:
  case Opcode::LOAD_GLOBAL:
  case Opcode::LOAD_CONST:
  case Opcode::LOAD_STRING:
  case Opcode::STORE_LOCAL:
  case Opcode::STORE_GLOBAL:
  case Opcode::CALL_FUNCTION:
    return true;
  default:
    return false;
  }
}

bool HasNoOperand(Opcode op) {
  switch (op) {
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

void ByteCode::Check() const {
  auto checked = false;
  if (HasIntOperand(opcode)) {
    assert(int_arg.has_value());
    checked = true;
  }
  if (HasStrOperand(opcode)) {
    assert(str_arg.has_value());
    checked = true;
  }
  if (HasNoOperand(opcode)) {
    assert(!int_arg.has_value() && !str_arg.has_value());
    checked = true;
  }
  assert(!checked && "unchecked opcode!");
}

void ByteCode::Format(std::ostream &os) const {
  os << "ByteCode(" << opcode << ", ";
  if (int_arg)
    os << "int_arg=" << *int_arg << ", ";
  if (str_arg)
    os << "str_arg=" << Quote(*str_arg) << ", ";
  os << "offset=" << offset << ", ";
  os << "lineno=" << lineno << ")";
}
