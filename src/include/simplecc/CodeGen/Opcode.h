#ifndef SIMPLECC_CODEGEN_OPCODE_H
#define SIMPLECC_CODEGEN_OPCODE_H
#include <iostream>

namespace simplecc {
enum class Opcode {
  LOAD_LOCAL,
  LOAD_GLOBAL,
  LOAD_CONST,
  LOAD_STRING,
  STORE_LOCAL,
  STORE_GLOBAL,
  BINARY_ADD,
  BINARY_SUB,
  BINARY_MULTIPLY,
  BINARY_DIVIDE,
  UNARY_POSITIVE,
  UNARY_NEGATIVE,
  CALL_FUNCTION,
  RETURN_VALUE,
  RETURN_NONE,
  PRINT_STRING,
  PRINT_CHARACTER,
  PRINT_INTEGER,
  PRINT_NEWLINE,
  READ_INTEGER,
  READ_CHARACTER,
  BINARY_SUBSCR,
  STORE_SUBSCR,
  JUMP_IF_TRUE,
  JUMP_IF_FALSE,
  JUMP_FORWARD,
  JUMP_IF_NOT_EQUAL,
  JUMP_IF_EQUAL,
  JUMP_IF_GREATER,
  JUMP_IF_GREATER_EQUAL,
  JUMP_IF_LESS,
  JUMP_IF_LESS_EQUAL,
  POP_TOP
};

const char *CStringFromOpcode(Opcode val);

inline std::ostream &operator<<(std::ostream &os, Opcode val) {
  return os << CStringFromOpcode(val);
}

} // namespace simplecc
#endif