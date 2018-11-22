
#include "Opcode.h"

const char *CStringFromOpcode(Opcode val) {
  switch (val) {
  case Opcode::LOAD_LOCAL:
    return "Opcode::LOAD_LOCAL";
  case Opcode::LOAD_GLOBAL:
    return "Opcode::LOAD_GLOBAL";
  case Opcode::LOAD_CONST:
    return "Opcode::LOAD_CONST";
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
  case Opcode::PRINT_STRING:
    return "Opcode::PRINT_STRING";
  case Opcode::PRINT_CHARACTER:
    return "Opcode::PRINT_CHARACTER";
  case Opcode::PRINT_INTEGER:
    return "Opcode::PRINT_INTEGER";
  case Opcode::BINARY_SUBSCR:
    return "Opcode::BINARY_SUBSCR";
  case Opcode::STORE_SUBSCR:
    return "Opcode::STORE_SUBSCR";
  case Opcode::COMPARE_AND_JUMP:
    return "Opcode::COMPARE_AND_JUMP";
  case Opcode::JUMP_FORWARD:
    return "Opcode::JUMP_FORWARD";
  }
}