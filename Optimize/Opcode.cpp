
#include "Opcode.h"
namespace simplecompiler {
const char *CStringFromOpcode(Opcode val) {
  switch (val) {
  case Opcode::LOAD_LOCAL:
    return "Opcode::LOAD_LOCAL";
  case Opcode::LOAD_GLOBAL:
    return "Opcode::LOAD_GLOBAL";
  case Opcode::LOAD_CONST:
    return "Opcode::LOAD_CONST";
  case Opcode::LOAD_STRING:
    return "Opcode::LOAD_STRING";
  case Opcode::STORE_LOCAL:
    return "Opcode::STORE_LOCAL";
  case Opcode::STORE_GLOBAL:
    return "Opcode::STORE_GLOBAL";
  case Opcode::BINARY_ADD:
    return "Opcode::BINARY_ADD";
  case Opcode::BINARY_SUB:
    return "Opcode::BINARY_SUB";
  case Opcode::BINARY_MULTIPLY:
    return "Opcode::BINARY_MULTIPLY";
  case Opcode::BINARY_DIVIDE:
    return "Opcode::BINARY_DIVIDE";
  case Opcode::UNARY_POSITIVE:
    return "Opcode::UNARY_POSITIVE";
  case Opcode::UNARY_NEGATIVE:
    return "Opcode::UNARY_NEGATIVE";
  case Opcode::CALL_FUNCTION:
    return "Opcode::CALL_FUNCTION";
  case Opcode::RETURN_VALUE:
    return "Opcode::RETURN_VALUE";
  case Opcode::RETURN_NONE:
    return "Opcode::RETURN_NONE";
  case Opcode::PRINT_STRING:
    return "Opcode::PRINT_STRING";
  case Opcode::PRINT_CHARACTER:
    return "Opcode::PRINT_CHARACTER";
  case Opcode::PRINT_INTEGER:
    return "Opcode::PRINT_INTEGER";
  case Opcode::PRINT_NEWLINE:
    return "Opcode::PRINT_NEWLINE";
  case Opcode::READ_INTEGER:
    return "Opcode::READ_INTEGER";
  case Opcode::READ_CHARACTER:
    return "Opcode::READ_CHARACTER";
  case Opcode::BINARY_SUBSCR:
    return "Opcode::BINARY_SUBSCR";
  case Opcode::STORE_SUBSCR:
    return "Opcode::STORE_SUBSCR";
  case Opcode::JUMP_IF_TRUE:
    return "Opcode::JUMP_IF_TRUE";
  case Opcode::JUMP_IF_FALSE:
    return "Opcode::JUMP_IF_FALSE";
  case Opcode::JUMP_FORWARD:
    return "Opcode::JUMP_FORWARD";
  case Opcode::JUMP_IF_NOT_EQUAL:
    return "Opcode::JUMP_IF_NOT_EQUAL";
  case Opcode::JUMP_IF_EQUAL:
    return "Opcode::JUMP_IF_EQUAL";
  case Opcode::JUMP_IF_GREATER:
    return "Opcode::JUMP_IF_GREATER";
  case Opcode::JUMP_IF_GREATER_EQUAL:
    return "Opcode::JUMP_IF_GREATER_EQUAL";
  case Opcode::JUMP_IF_LESS:
    return "Opcode::JUMP_IF_LESS";
  case Opcode::JUMP_IF_LESS_EQUAL:
    return "Opcode::JUMP_IF_LESS_EQUAL";
  case Opcode::POP_TOP:
    return "Opcode::POP_TOP";
  }
}
} // namespace simplecompiler