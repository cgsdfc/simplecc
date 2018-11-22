
#include "Opcode.h"

// Hard coded mappings
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
  case BasicTypeKind::Void:
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
  case Opcode::READ_INTEGER:
    return "Opcode::READ_INTEGER";
  case Opcode::READ_CHARACTER:
    return "Opcode::READ_CHARACTER";
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