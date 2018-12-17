
#include "simplecc/Opcode.h"
namespace simplecc {
const char *CStringFromOpcode(Opcode val) {
  switch (val) {
  case Opcode::LOAD_LOCAL:
    return "LOAD_LOCAL";
  case Opcode::LOAD_GLOBAL:
    return "LOAD_GLOBAL";
  case Opcode::LOAD_CONST:
    return "LOAD_CONST";
  case Opcode::LOAD_STRING:
    return "LOAD_STRING";
  case Opcode::STORE_LOCAL:
    return "STORE_LOCAL";
  case Opcode::STORE_GLOBAL:
    return "STORE_GLOBAL";
  case Opcode::BINARY_ADD:
    return "BINARY_ADD";
  case Opcode::BINARY_SUB:
    return "BINARY_SUB";
  case Opcode::BINARY_MULTIPLY:
    return "BINARY_MULTIPLY";
  case Opcode::BINARY_DIVIDE:
    return "BINARY_DIVIDE";
  case Opcode::UNARY_POSITIVE:
    return "UNARY_POSITIVE";
  case Opcode::UNARY_NEGATIVE:
    return "UNARY_NEGATIVE";
  case Opcode::CALL_FUNCTION:
    return "CALL_FUNCTION";
  case Opcode::RETURN_VALUE:
    return "RETURN_VALUE";
  case Opcode::RETURN_NONE:
    return "RETURN_NONE";
  case Opcode::PRINT_STRING:
    return "PRINT_STRING";
  case Opcode::PRINT_CHARACTER:
    return "PRINT_CHARACTER";
  case Opcode::PRINT_INTEGER:
    return "PRINT_INTEGER";
  case Opcode::PRINT_NEWLINE:
    return "PRINT_NEWLINE";
  case Opcode::READ_INTEGER:
    return "READ_INTEGER";
  case Opcode::READ_CHARACTER:
    return "READ_CHARACTER";
  case Opcode::BINARY_SUBSCR:
    return "BINARY_SUBSCR";
  case Opcode::STORE_SUBSCR:
    return "STORE_SUBSCR";
  case Opcode::JUMP_IF_TRUE:
    return "JUMP_IF_TRUE";
  case Opcode::JUMP_IF_FALSE:
    return "JUMP_IF_FALSE";
  case Opcode::JUMP_FORWARD:
    return "JUMP_FORWARD";
  case Opcode::JUMP_IF_NOT_EQUAL:
    return "JUMP_IF_NOT_EQUAL";
  case Opcode::JUMP_IF_EQUAL:
    return "JUMP_IF_EQUAL";
  case Opcode::JUMP_IF_GREATER:
    return "JUMP_IF_GREATER";
  case Opcode::JUMP_IF_GREATER_EQUAL:
    return "JUMP_IF_GREATER_EQUAL";
  case Opcode::JUMP_IF_LESS:
    return "JUMP_IF_LESS";
  case Opcode::JUMP_IF_LESS_EQUAL:
    return "JUMP_IF_LESS_EQUAL";
  case Opcode::POP_TOP:
    return "POP_TOP";
  }
}
} // namespace simplecc