#ifndef BYTE_CODE_H
#define BYTE_CODE_H

#include "Opcode.h"
#include <cassert>
#include <optional>

namespace simplecompiler {
class ByteCode {
  Opcode opcode;
  // lineno in source file
  unsigned lineno;
  std::optional<int> int_arg;
  const char *str_arg;
  // offset in the instruction buffer
  int offset;

public:
  ByteCode(Opcode opcode) : opcode(opcode), int_arg(), str_arg() {}

  ByteCode(Opcode opcode, int arg) : opcode(opcode), int_arg(arg), str_arg() {}

  ByteCode(Opcode opcode, const char *arg)
      : opcode(opcode), int_arg(), str_arg(arg) {}

  ByteCode(Opcode opcode, int int_arg, const char *str_arg)
      : opcode(opcode), int_arg(int_arg), str_arg(str_arg) {}

  void SetLineno(unsigned lineno) { this->lineno = lineno; }

  void SetTarget(int target) { int_arg = target; }

  void SetOffset(int offset) { this->offset = offset; }

  Opcode GetOpcode() const { return opcode; }

  int GetIntOperand() const {
    assert(int_arg.has_value());
    return *int_arg;
  }

  const char *GetStrOperand() const {
    assert(str_arg);
    return str_arg;
  }

  int GetOffset() const { return offset; }

  void Check() const;
  void Format(std::ostream &os) const;
};

inline std::ostream &operator<<(std::ostream &os, const ByteCode &c) {
  c.Format(os);
  return os;
}

bool HasIntOperand(Opcode op);
bool HasStrOperand(Opcode op);
bool HasNoOperand(Opcode op);
bool IsJumpXXX(Opcode op);
} // namespace simplecompiler
#endif
