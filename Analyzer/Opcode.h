
#ifndef OPCODE_H
#define OPCODE_H
#include "AST.h"
#include "symtable.h"
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
  RETURN_NONE,
  PRINT_STRING,
  PRINT_CHARACTER,
  PRINT_INTEGER,
  READ_INTEGER,
  READ_CHARACTER,
  BINARY_SUBSCR,
  STORE_SUBSCR,
  JUMP_IF_TRUE,
  JUMP_FORWARD,
  JUMP_IF_NOT_EQUAL,
  JUMP_IF_EQUAL,
  JUMP_IF_GREATER,
  JUMP_IF_GREATER_EQUAL,
  JUMP_IF_LESS,
  JUMP_IF_LESS_EQUAL
};

const char *CStringFromOpcode(Opcode val);
Opcode MakeSubScr(ExprContextKind ctx);
Opcode MakeStore(Scope scope);
Opcode MakeLoad(Scope scope);
Opcode MakeRead(BasicTypeKind type);
Opcode MakePrint(BasicTypeKind type);
Opcode MakeBinary(OperatorKind oper);
Opcode MakeUnary(UnaryopKind oper);
Opcode MakeJump(OperatorKind oper);
inline std::ostream &operator<<(std::ostream &os, Opcode val) {
  return os << CStringFromOpcode(val);
}

#endif