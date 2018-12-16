#ifndef BYTE_CODE_H
#define BYTE_CODE_H

#include "Opcode.h"

#include <cassert>
#include <iostream>

namespace simplecompiler {
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
  bool HasIntOperand() const { return HasIntOperand(GetOpcode()); }

  static bool HasStrOperand(Opcode op);
  bool HasStrOperand() const { return HasStrOperand(GetOpcode()); }

  static bool HasNoOperand(Opcode op);
  bool HasNoOperand() const { return HasNoOperand(GetOpcode()); }

  static bool IsJumpXXX(Opcode op);
  bool IsJumpXXX() const { return IsJumpXXX(GetOpcode()); }

  void SetSourceLineno(unsigned Line) { SourceLineno = Line; }
  unsigned GetSourceLineno() const { return SourceLineno; }

  /// set the jump target for this ByteCode if this is a jump.
  void SetJumpTarget(unsigned Target) {
    assert(IsJumpXXX() && "SetJumpTarget() on non-jump ByteCode");
    IntOperand = Target;
  }

  unsigned GetJumpTarget() const {
    assert(IsJumpXXX() && "GetJumpTarget() on non-jump ByteCode");
    return IntOperand;
  }

  void SetByteCodeOffset(unsigned Offset) { ByteCodeOffset = Offset; }
  unsigned GetByteCodeOffset() const { return ByteCodeOffset; }

  Opcode GetOpcode() const { return Op; }

  int GetIntOperand() const {
    assert(HasIntOperand());
    return IntOperand;
  }
  void SetIntOperand(int Val) {
    assert(HasIntOperand());
    IntOperand = Val;
  }

  const char *GetStrOperand() const {
    assert(HasStrOperand());
    return StrOperand;
  }
  void SetStrOperand(const char *Val) {
    assert(HasStrOperand());
    StrOperand = Val;
  }

  void Format(std::ostream &os) const;
};

inline std::ostream &operator<<(std::ostream &O, const ByteCode &c) {
  c.Format(O);
  return O;
}

} // namespace simplecompiler
#endif
