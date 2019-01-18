#include "simplecc/CodeGen/ByteCodeBuilder.h"

using namespace simplecc;

ByteCodeBuilder::Opcode ByteCodeBuilder::MakeSubScr(ExprContextKind ctx) {
  switch (ctx) {
  case ExprContextKind::Load:
    return ByteCode::BINARY_SUBSCR;
  case ExprContextKind::Store:
    return ByteCode::STORE_SUBSCR;
  }
}

ByteCodeBuilder::Opcode ByteCodeBuilder::MakeLoad(Scope scope) {
  switch (scope) {
  case Scope::Global:
    return ByteCode::LOAD_GLOBAL;
  case Scope::Local:
    return ByteCode::LOAD_LOCAL;
  }
}

ByteCodeBuilder::Opcode ByteCodeBuilder::MakeStore(Scope scope) {
  switch (scope) {
  case Scope::Global:
    return ByteCode::STORE_GLOBAL;
  case Scope::Local:
    return ByteCode::STORE_LOCAL;
  }
}

ByteCodeBuilder::Opcode ByteCodeBuilder::MakeRead(BasicTypeKind type) {
  switch (type) {
  case BasicTypeKind::Character:
    return ByteCode::READ_CHARACTER;
  case BasicTypeKind::Int:
    return ByteCode::READ_INTEGER;
  default:
    assert(false);
  }
}

ByteCodeBuilder::Opcode ByteCodeBuilder::MakePrint(BasicTypeKind type) {
  switch (type) {
  case BasicTypeKind::Character:
    return ByteCode::PRINT_CHARACTER;
  case BasicTypeKind::Int:
    return ByteCode::PRINT_INTEGER;
  default:
    assert(false);
  }
}

ByteCodeBuilder::Opcode ByteCodeBuilder::MakeBinary(BinaryOpKind Op) {
  switch (Op) {
  case BinaryOpKind::Add:
    return ByteCode::BINARY_ADD;
  case BinaryOpKind::Sub:
    return ByteCode::BINARY_SUB;
  case BinaryOpKind::Mult:
    return ByteCode::BINARY_MULTIPLY;
  case BinaryOpKind::Div:
    return ByteCode::BINARY_DIVIDE;
  default:
    assert(false);
  }
}

ByteCodeBuilder::Opcode ByteCodeBuilder::MakeUnary(UnaryOpKind Op) {
  switch (Op) {
  case UnaryOpKind::UAdd:
    return ByteCode::UNARY_POSITIVE;
  case UnaryOpKind::USub:
    return ByteCode::UNARY_NEGATIVE;
  }
}

ByteCodeBuilder::Opcode ByteCodeBuilder::MakeJumpNegative(BinaryOpKind Op) {
  switch (Op) {
  case BinaryOpKind::NotEq:
    return ByteCode::JUMP_IF_EQUAL;
  case BinaryOpKind::Eq:
    return ByteCode::JUMP_IF_NOT_EQUAL;
  case BinaryOpKind::GtE:
    return ByteCode::JUMP_IF_LESS;
  case BinaryOpKind::Gt:
    return ByteCode::JUMP_IF_LESS_EQUAL;
  case BinaryOpKind::LtE:
    return ByteCode::JUMP_IF_GREATER;
  case BinaryOpKind::Lt:
    return ByteCode::JUMP_IF_GREATER_EQUAL;
  default:
    assert(false);
  }
}

ByteCodeBuilder::Opcode ByteCodeBuilder::MakeJump(BinaryOpKind Op) {
  switch (Op) {
  case BinaryOpKind::Eq:
    return ByteCode::JUMP_IF_EQUAL;
  case BinaryOpKind::NotEq:
    return ByteCode::JUMP_IF_NOT_EQUAL;
  case BinaryOpKind::Lt:
    return ByteCode::JUMP_IF_LESS;
  case BinaryOpKind::LtE:
    return ByteCode::JUMP_IF_LESS_EQUAL;
  case BinaryOpKind::Gt:
    return ByteCode::JUMP_IF_GREATER;
  case BinaryOpKind::GtE:
    return ByteCode::JUMP_IF_GREATER_EQUAL;
  default:
    assert(false);
  }
}

unsigned ByteCodeBuilder::Insert(ByteCode Code) {
  /// get the function being built.
  ByteCodeFunction &TheFunction = *getInsertPoint();

  /// Fill in other members of Code.
  auto Off = TheFunction.size();
  Code.setSourceLineno(getLineno());
  Code.setByteCodeOffset(Off);

  /// Insert Code at the back of the function.
  TheFunction.getByteCodeList().push_back(Code);
  return Off;
}

void ByteCodeBuilder::setJumpTargetAt(unsigned Idx, unsigned Target) {
  ByteCode &TheCode = getInsertPoint()->getByteCodeAt(Idx);
  TheCode.setJumpTarget(Target);
}