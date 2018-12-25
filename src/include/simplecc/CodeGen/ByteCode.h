#ifndef SIMPLECC_CODEGEN_BYTECODE_H
#define SIMPLECC_CODEGEN_BYTECODE_H

#include "simplecc/CodeGen/Opcode.h"

#include <cassert>
#include <iostream>

namespace simplecc {
/// This class is a simplified byte code implementatoin.
/// A ByteCode instance consists of an Opcode, an optional integer
/// operand and an optional string operand. It also has information
/// that connects compiled form to source code -- a kind of debugging
/// information, namely the source line No and offset in the byte code
/// stream of a function.
///
/// ByteCode can be translated to MIPS with a software-emulated stack
/// without much difficulty. However, it cannot be subject to meaningful
/// optimization. And the resultant MIPS code has a lot of stack traffic
/// that makes it less than optimal.
///
class ByteCode {
  /// Core IR information.
  Opcode Op;
  int IntOperand = 0;
  const char *StrOperand = nullptr;

  /// Debugging Information.
  // lineno in source file
  unsigned SourceLineno = 0;

  // offset in the ByteCode stream.
  unsigned ByteCodeOffset = 0;

  /// Private. Use Create() instead.
  ByteCode(Opcode Op) : Op(Op) {}

public:
  ~ByteCode() = default;

  /// Factories to create ByteCode instance correctly.
  static ByteCode Create(Opcode Op) { return ByteCode(Op); }

  static ByteCode Create(Opcode Op, int Val);

  static ByteCode Create(Opcode Op, const char *Val);

  static ByteCode Create(Opcode Op, const char *Str, int Int);

  /// ByteCode operand inspectation.
  static bool HasIntOperand(Opcode op);
  bool HasIntOperand() const { return HasIntOperand(getOpcode()); }

  static bool HasStrOperand(Opcode op);
  bool HasStrOperand() const { return HasStrOperand(getOpcode()); }

  static bool HasNoOperand(Opcode op);
  bool HasNoOperand() const { return HasNoOperand(getOpcode()); }

  static bool IsJumpXXX(Opcode op);
  bool IsJumpXXX() const { return IsJumpXXX(getOpcode()); }

  void setSourceLineno(unsigned Line) { SourceLineno = Line; }
  unsigned getSourceLineno() const { return SourceLineno; }

  /// set the jump target for this ByteCode if this is a jump.
  void setJumpTarget(unsigned Target) {
    assert(IsJumpXXX() && "SetJumpTarget() on non-jump ByteCode");
    IntOperand = Target;
  }

  unsigned getJumpTarget() const {
    assert(IsJumpXXX() && "GetJumpTarget() on non-jump ByteCode");
    assert(IntOperand >= 0 && "Negative JumpTarget!");
    return static_cast<unsigned>(IntOperand);
  }

  void setByteCodeOffset(unsigned Offset) { ByteCodeOffset = Offset; }
  unsigned getByteCodeOffset() const { return ByteCodeOffset; }

  Opcode getOpcode() const { return Op; }
  const char *getOpcodeName() const { return CStringFromOpcode(getOpcode()); }

  int getIntOperand() const {
    assert(HasIntOperand());
    return IntOperand;
  }
  void setIntOperand(int Val) {
    assert(HasIntOperand());
    IntOperand = Val;
  }

  const char *getStrOperand() const {
    assert(HasStrOperand());
    return StrOperand;
  }
  void setStrOperand(const char *Val) {
    assert(HasStrOperand());
    StrOperand = Val;
  }

  void Format(std::ostream &os) const;
};

inline std::ostream &operator<<(std::ostream &O, const ByteCode &c) {
  c.Format(O);
  return O;
}

} // namespace simplecc
#endif
