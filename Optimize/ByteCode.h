#ifndef BYTE_CODE_H
#define BYTE_CODE_H

#include "Opcode.h"

#include <cassert>
#include <iostream>

namespace simplecompiler {
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

  ByteCode(Opcode Op) : Op(Op) {}
public:
  ~ByteCode() = default;

  /// Factories to create ByteCode instance correctly.
  static ByteCode Create(Opcode Op) {
    return ByteCode(Op);
  }

  static ByteCode Create(Opcode Op, int Val) {
    ByteCode B(Op);
    B.SetIntOperand(Val);
    return B;
  }

  static ByteCode Create(Opcode Op, const char *Val) {
    ByteCode B(Op);
    B.SetStrOperand(Val);
    return B;
  }

  static ByteCode Create(Opcode Op, const char *Str, int Int) {
    ByteCode B(Op);
    B.SetStrOperand(Str);
    B.SetIntOperand(Int);
    return B;
  }

  /// ByteCode property inspectation.
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
