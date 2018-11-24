
#include "Opcode.h"

// Hard coded mappings

Opcode MakeSubScr(ExprContextKind ctx) {
  switch (ctx) {
  case ExprContextKind::Load:
    return Opcode::BINARY_SUBSCR;
  case ExprContextKind::Store:
    return Opcode::STORE_SUBSCR;
  }
}

Opcode MakeLoad(Scope scope) {
  switch (scope) {
  case Scope::Global:
    return Opcode::LOAD_GLOBAL;
  case Scope::Local:
    return Opcode::LOAD_LOCAL;
  }
}

Opcode MakeStore(Scope scope) {
  switch (scope) {
  case Scope::Global:
    return Opcode::STORE_GLOBAL;
  case Scope::Local:
    return Opcode::STORE_LOCAL;
  }
}

Opcode MakeRead(BasicTypeKind type) {
  switch (type) {
  case BasicTypeKind::Character:
    return Opcode::READ_CHARACTER;
  case BasicTypeKind::Int:
    return Opcode::READ_INTEGER;
  default:
    assert(false);
  }
}

Opcode MakePrint(BasicTypeKind type) {
  switch (type) {
  case BasicTypeKind::Character:
    return Opcode::PRINT_CHARACTER;
  case BasicTypeKind::Int:
    return Opcode::PRINT_INTEGER;
  default:
    assert(false);
  }
}

Opcode MakeBinary(OperatorKind oper) {
  switch (oper) {
  case OperatorKind::Add:
    return Opcode::BINARY_ADD;
  case OperatorKind::Sub:
    return Opcode::BINARY_SUB;
  case OperatorKind::Mult:
    return Opcode::BINARY_MULTIPLY;
  case OperatorKind::Div:
    return Opcode::BINARY_DIVIDE;
  default:
    assert(false);
  }
}

Opcode MakeUnary(UnaryopKind oper) {
  switch (oper) {
  case UnaryopKind::UAdd:
    return Opcode::UNARY_POSITIVE;
  case UnaryopKind::USub:
    return Opcode::UNARY_NEGATIVE;
  }
}

Opcode MakeJumpNegative(OperatorKind oper) {
  switch (oper) {
  case OperatorKind::NotEq:
    return Opcode::JUMP_IF_EQUAL;
  case OperatorKind::Eq:
    return Opcode::JUMP_IF_NOT_EQUAL;
  case OperatorKind::GtE:
    return Opcode::JUMP_IF_LESS;
  case OperatorKind::Gt:
    return Opcode::JUMP_IF_LESS_EQUAL;
  case OperatorKind::LtE:
    return Opcode::JUMP_IF_GREATER;
  case OperatorKind::Lt:
    return Opcode::JUMP_IF_GREATER_EQUAL;
  default:
    assert(false);
  }
}

Opcode MakeJump(OperatorKind oper) {
  switch (oper) {
  case OperatorKind::Eq:
    return Opcode::JUMP_IF_EQUAL;
  case OperatorKind::NotEq:
    return Opcode::JUMP_IF_NOT_EQUAL;
  case OperatorKind::Lt:
    return Opcode::JUMP_IF_LESS;
  case OperatorKind::LtE:
    return Opcode::JUMP_IF_LESS_EQUAL;
  case OperatorKind::Gt:
    return Opcode::JUMP_IF_GREATER;
  case OperatorKind::GtE:
    return Opcode::JUMP_IF_GREATER_EQUAL;
  default:
    assert(false);
  }
}

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
  case Opcode::UNARY_NOT:
    return "Opcode::UNARY_NOT";
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