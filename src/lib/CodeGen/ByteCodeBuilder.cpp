// MIT License
// 
// Copyright (c) 2018 Cong Feng.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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