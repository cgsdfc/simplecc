
#ifndef OPCODE_H
#define OPCODE_H
#include <iostream>

enum class Opcode {
  LOAD_LOCAL,
  LOAD_GLOBAL,
  LOAD_CONST,
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
  PRINT_STRING,
  PRINT_CHARACTER,
  PRINT_INTEGER,
  BINARY_SUBSCR,
  STORE_SUBSCR,
  COMPARE_AND_JUMP,
  JUMP_FORWARD
};

const char *CStringFromOpcode(Opcode val);

inline std::ostream &operator<<(std::ostream &os, Opcode val) {
  return os << CStringFromOpcode(val);
}

#endif