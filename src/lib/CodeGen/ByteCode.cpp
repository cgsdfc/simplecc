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

#include "simplecc/CodeGen/ByteCode.h"
#include "simplecc/Support/ErrorManager.h"
#include <iomanip>

namespace simplecc {

ByteCode ByteCode::Create(Opcode Op, int Val) {
  ByteCode B(Op);
  B.setIntOperand(Val);
  return B;
}

ByteCode ByteCode::Create(Opcode Op, const char *Val) {
  ByteCode B(Op);
  B.setStrOperand(Val);
  return B;
}

ByteCode ByteCode::Create(Opcode Op, const char *Str, int Int) {
  ByteCode B(Op);
  B.setStrOperand(Str);
  B.setIntOperand(Int);
  return B;
}

bool ByteCode::IsJump(Opcode Op) {
  switch (Op) {
  default:return false;
#define HANDLE_JUMP(opcode, camelName) case Opcode::opcode: return true;
#include "simplecc/CodeGen/Opcode.def"
  }
}

bool ByteCode::HasIntOperand(Opcode Op) {
  switch (Op) {
  default:return false;
#define HAS_INT_OPERAND_ONLY(OP, NAME) case OP: return true;
#define HAS_INT_AND_STR_OPERAND(OP, NAME) case OP: return true;
#include "simplecc/CodeGen/Opcode.def"
  }
}

bool ByteCode::HasStrOperand(Opcode Op) {
  switch (Op) {
  default:return false;
#define HAS_STR_OPERAND_ONLY(OP, NAME) case OP:
#define HAS_INT_AND_STR_OPERAND(OP, NAME) case OP:
#include "simplecc/CodeGen/Opcode.def"
    return true;
  }
}

bool ByteCode::HasNoOperand(Opcode Op) {
  switch (Op) {
  default:return false;
#define HAS_NO_OPERAND(OP, NAME) case OP:
#include "simplecc/CodeGen/Opcode.def"
    return true;
  }
}

void ByteCode::Format(std::ostream &O) const {
  O << std::left << std::setw(4) << getByteCodeOffset();
  O << std::left << std::setw(25) << getOpcodeName();

  if (HasIntOperand()) {
    O << std::setw(10) << getIntOperand();
  }

  if (HasStrOperand()) {
    O << std::setw(20) << getStrOperand();
  }
}

const char *ByteCode::getOpcodeName(unsigned Op) {
  switch (Op) {
  default:assert(false && "Invalid Opcode");
#define HANDLE_OPCODE(opcode, camelName)                                       \
  case Opcode::opcode:                                                         \
    return #opcode;
#include "simplecc/CodeGen/Opcode.def"
  }
}

} // namespace simplecc