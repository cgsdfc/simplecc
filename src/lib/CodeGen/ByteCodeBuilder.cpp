#include "simplecc/CodeGen/ByteCodeBuilder.h"
#include "simplecc/CodeGen/ByteCode.h"
#include "simplecc/CodeGen/ByteCodeFunction.h"
#include "simplecc/Parse/AST.h"

using namespace simplecc;

unsigned ByteCodeBuilder::MakeSubScr(ExprContextKind ctx) {
  switch (ctx) {
  case ExprContextKind::Load:
    return ByteCode::BINARY_SUBSCR;
  case ExprContextKind::Store:
    return ByteCode::STORE_SUBSCR;
  }
}

unsigned ByteCodeBuilder::MakeLoad(Scope scope) {
  switch (scope) {
  case Scope::Global:
    return ByteCode::LOAD_GLOBAL;
  case Scope::Local:
    return ByteCode::LOAD_LOCAL;
  }
}

unsigned ByteCodeBuilder::MakeStore(Scope scope) {
  switch (scope) {
  case Scope::Global:
    return ByteCode::STORE_GLOBAL;
  case Scope::Local:
    return ByteCode::STORE_LOCAL;
  }
}

unsigned ByteCodeBuilder::MakeRead(BasicTypeKind type) {
  switch (type) {
  case BasicTypeKind::Character:
    return ByteCode::READ_CHARACTER;
  case BasicTypeKind::Int:
    return ByteCode::READ_INTEGER;
  default:
    assert(false);
  }
}

unsigned ByteCodeBuilder::MakePrint(BasicTypeKind type) {
  switch (type) {
  case BasicTypeKind::Character:
    return ByteCode::PRINT_CHARACTER;
  case BasicTypeKind::Int:
    return ByteCode::PRINT_INTEGER;
  default:
    assert(false);
  }
}

unsigned ByteCodeBuilder::MakeBinary(OperatorKind Op) {
  switch (Op) {
  case OperatorKind::Add:
    return ByteCode::BINARY_ADD;
  case OperatorKind::Sub:
    return ByteCode::BINARY_SUB;
  case OperatorKind::Mult:
    return ByteCode::BINARY_MULTIPLY;
  case OperatorKind::Div:
    return ByteCode::BINARY_DIVIDE;
  default:
    assert(false);
  }
}

unsigned ByteCodeBuilder::MakeUnary(UnaryopKind Op) {
  switch (Op) {
  case UnaryopKind::UAdd:
    return ByteCode::UNARY_POSITIVE;
  case UnaryopKind::USub:
    return ByteCode::UNARY_NEGATIVE;
  }
}

unsigned ByteCodeBuilder::MakeJumpNegative(OperatorKind Op) {
  switch (Op) {
  case OperatorKind::NotEq:
    return ByteCode::JUMP_IF_EQUAL;
  case OperatorKind::Eq:
    return ByteCode::JUMP_IF_NOT_EQUAL;
  case OperatorKind::GtE:
    return ByteCode::JUMP_IF_LESS;
  case OperatorKind::Gt:
    return ByteCode::JUMP_IF_LESS_EQUAL;
  case OperatorKind::LtE:
    return ByteCode::JUMP_IF_GREATER;
  case OperatorKind::Lt:
    return ByteCode::JUMP_IF_GREATER_EQUAL;
  default:
    assert(false);
  }
}

unsigned ByteCodeBuilder::MakeJump(OperatorKind Op) {
  switch (Op) {
  case OperatorKind::Eq:
    return ByteCode::JUMP_IF_EQUAL;
  case OperatorKind::NotEq:
    return ByteCode::JUMP_IF_NOT_EQUAL;
  case OperatorKind::Lt:
    return ByteCode::JUMP_IF_LESS;
  case OperatorKind::LtE:
    return ByteCode::JUMP_IF_LESS_EQUAL;
  case OperatorKind::Gt:
    return ByteCode::JUMP_IF_GREATER;
  case OperatorKind::GtE:
    return ByteCode::JUMP_IF_GREATER_EQUAL;
  default:
    assert(false);
  }
}

/// Insert a ByteCode into the back of InsertPoint.
/// Return the offset of the inserted ByteCode.
//
unsigned ByteCodeBuilder::Insert(ByteCode Code) {
  /// get the function being built.
  ByteCodeFunction &TheFunction = *getInsertPoint();

  /// Fill in other members of Code.
  auto Off = TheFunction.size();
  Code.setSourceLineno(getLineNo());
  Code.setByteCodeOffset(Off);

  /// Insert Code at the back of the function.
  TheFunction.getByteCodeList().push_back(Code);
  return Off;
}

void ByteCodeBuilder::setJumpTargetAt(unsigned Idx, unsigned Target) {
  ByteCode &TheCode = getInsertPoint()->getByteCodeAt(Idx);
  TheCode.setJumpTarget(Target);
}

unsigned ByteCodeBuilder::getSize() const { return getInsertPoint()->size(); }
