#include "simplecc/ByteCodeBuilder.h"
#include "simplecc/AST.h"
#include "simplecc/ByteCode.h"
#include "simplecc/ByteCodeFunction.h"

using namespace simplecc;

Opcode ByteCodeBuilder::MakeSubScr(ExprContextKind ctx) {
  switch (ctx) {
  case ExprContextKind::Load:
    return Opcode::BINARY_SUBSCR;
  case ExprContextKind::Store:
    return Opcode::STORE_SUBSCR;
  }
}

Opcode ByteCodeBuilder::MakeLoad(Scope scope) {
  switch (scope) {
  case Scope::Global:
    return Opcode::LOAD_GLOBAL;
  case Scope::Local:
    return Opcode::LOAD_LOCAL;
  }
}

Opcode ByteCodeBuilder::MakeStore(Scope scope) {
  switch (scope) {
  case Scope::Global:
    return Opcode::STORE_GLOBAL;
  case Scope::Local:
    return Opcode::STORE_LOCAL;
  }
}

Opcode ByteCodeBuilder::MakeRead(BasicTypeKind type) {
  switch (type) {
  case BasicTypeKind::Character:
    return Opcode::READ_CHARACTER;
  case BasicTypeKind::Int:
    return Opcode::READ_INTEGER;
  default:
    assert(false);
  }
}

Opcode ByteCodeBuilder::MakePrint(BasicTypeKind type) {
  switch (type) {
  case BasicTypeKind::Character:
    return Opcode::PRINT_CHARACTER;
  case BasicTypeKind::Int:
    return Opcode::PRINT_INTEGER;
  default:
    assert(false);
  }
}

Opcode ByteCodeBuilder::MakeBinary(OperatorKind Op) {
  switch (Op) {
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

Opcode ByteCodeBuilder::MakeUnary(UnaryopKind Op) {
  switch (Op) {
  case UnaryopKind::UAdd:
    return Opcode::UNARY_POSITIVE;
  case UnaryopKind::USub:
    return Opcode::UNARY_NEGATIVE;
  }
}

Opcode ByteCodeBuilder::MakeJumpNegative(OperatorKind Op) {
  switch (Op) {
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

Opcode ByteCodeBuilder::MakeJump(OperatorKind Op) {
  switch (Op) {
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
  TheFunction.GetByteCodeList().push_back(std::move(Code));
  return Off;
}

void ByteCodeBuilder::setJumpTargetAt(unsigned Idx, unsigned Target) {
  ByteCode &TheCode = getInsertPoint()->getByteCodeAt(Idx);
  TheCode.setJumpTarget(Target);
}

unsigned ByteCodeBuilder::getSize() const { return getInsertPoint()->size(); }
