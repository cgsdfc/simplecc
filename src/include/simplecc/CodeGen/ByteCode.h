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

#ifndef SIMPLECC_CODEGEN_BYTECODE_H
#define SIMPLECC_CODEGEN_BYTECODE_H
#include "simplecc/Support/Macros.h"
#include <cassert>
#include <iostream>

namespace simplecc {
/// @brief ByteCode is the primary intermediate representation (IR) of simplecc.
/// A ByteCode instance consists of an Opcode, an optional integer operand and an optional cstring operand.
///
/// Different Opcode demands different number and type of operands. For example, the ``JUMP_FORWARD`` Opcode
/// demands a int operand as its jump target. The ``LOAD_GLOBAL`` Opcode demands a cstring operand as its name.
/// Those properties about operands can be obtained by calling hasIntOperand(), hasStrOperand() and hasNoOperand() member functions.
/// If an Opcode demands a certain operands, one can get and set these operands by the calling the Operand accessors.
/// Otherwise, calling accessors on ByteCode that does not has a certain operands triggers an assertion.
///
/// ByteCode can be translated to MIPS backing by a software-emulated stack.
/// However, meaningful optimization cannot be applied to this form of IR and
/// the resultant machine code is rather slow.
class ByteCode {
public:
  /// Opcode for ByteCode.
  enum Opcode : unsigned {
#define HANDLE_OPCODE(opcode, camelName) opcode,
#include "simplecc/CodeGen/Opcode.def"
  };
private:
  Opcode Op;
  int IntOperand = 0;
  const char *StrOperand = nullptr;

  // Lineno in source file.
  unsigned SourceLineno = 0;

  // Offset in the ByteCode stream of a ByteCodeFunction.
  unsigned ByteCodeOffset = 0;

  /// Private. Use Create() instead.
  explicit ByteCode(Opcode Op) : Op(Op) {}

  /// Helpers.
  static bool HasStrOperand(Opcode op);
  static bool HasIntOperand(Opcode op);
  static bool IsJump(Opcode Op);
  static bool HasNoOperand(Opcode op);
  static const char *getOpcodeName(unsigned Op);
public:
  ~ByteCode() = default;
  ByteCode(const ByteCode &) = default;
  ByteCode(ByteCode &&) = default;

  /// Create a ByteCode with no operand.
  static ByteCode Create(Opcode Op) { return ByteCode(Op); }

  /// Create a ByteCode with an int operand.
  static ByteCode Create(Opcode Op, int Val);

  /// Create a ByteCode with a cstring operand.
  /// The pointer is a borrowed one and the actual storage must outlive the ByteCode instance.
  static ByteCode Create(Opcode Op, const char *Val);

  /// Create a ByteCode with both an int and a cstring operand.
  static ByteCode Create(Opcode Op, const char *Str, int Int);

  /// Return if this opcode has an int operand.
  bool HasIntOperand() const { return HasIntOperand(getOpcode()); }

  /// Return if this opcode has an cstring operand.
  bool HasStrOperand() const { return HasStrOperand(getOpcode()); }

  /// Return if this opcode has no operand.
  bool HasNoOperand() const { return HasNoOperand(getOpcode()); }

  /// Return if this has a jump Opcode.
  bool IsJump() const { return IsJump(getOpcode()); }

  /// Set the source lineno.
  void setSourceLineno(unsigned Line) { SourceLineno = Line; }

  /// Return the source lineno.
  unsigned getSourceLineno() const { return SourceLineno; }

  /// Set the jump target for this ByteCode if this is a jump.
  void setJumpTarget(unsigned Target) {
    assert(IsJump() && "not a jump!");
    IntOperand = Target;
  }

  /// Return the jump target if it is a jump.
  unsigned getJumpTarget() const {
    assert(IsJump() && "not a jump!");
    assert(IntOperand >= 0 && "negative jump target!");
    return static_cast<unsigned>(IntOperand);
  }

  /// Set the offset in the stream.
  void setByteCodeOffset(unsigned Offset) { ByteCodeOffset = Offset; }

  /// Return the offset in the stream.
  unsigned getByteCodeOffset() const { return ByteCodeOffset; }

  /// Return the Opcode.
  Opcode getOpcode() const { return Op; }

  /// Return the name of the Opcode.
  const char *getOpcodeName() const { return getOpcodeName(getOpcode()); }

  /// Return the int operand if has one.
  int getIntOperand() const {
    assert(HasIntOperand());
    return IntOperand;
  }

  /// Set the int operand if has one.
  void setIntOperand(int Val) {
    assert(HasIntOperand());
    IntOperand = Val;
  }

  /// Return the cstring operand if has one.
  const char *getStrOperand() const {
    assert(HasStrOperand());
    return StrOperand;
  }

  /// Set the cstring operand if has one.
  /// The pointer is a borrowed one and the actual storage must outlive the ByteCode instance.
  void setStrOperand(const char *Val) {
    assert(HasStrOperand());
    StrOperand = Val;
  }

  void Format(std::ostream &os) const;
};

DEFINE_INLINE_OUTPUT_OPERATOR(ByteCode)

} // namespace simplecc
#endif // SIMPLECC_CODEGEN_BYTECODE_H