#ifndef CODE_H
#define CODE_H

#include "Opcode.h"
#include <optional>

class ByteCode {
  Opcode opcode;
  std::optional<int> int_arg;
  std::optional<const char*> str_arg;
public:
  ByteCode(Opcode opcode): opcode(opcode), int_arg(), str_arg() {}

  ByteCode(Opcode opcode, int arg): opcode(opcode), int_arg(arg), str_arg() {}

  ByteCode(Opcode opcode, const char *arg):
    opcode(opcode), int_arg(), str_arg(arg) {}

  ByteCode(Opcode opcode, int int_arg, const char *str_arg):
    opcode(opcode), int_arg(int_arg), str_arg(str_arg) {}

  void SetTarget(int target) {
    int_arg = target;
  }

  Opcode GetOpcode() const {
    return opcode;
  }

  int GetIntOperand() const {
    assert(int_arg.has_value());
    return *int_arg;
  }

  const char *GetStrOperand() const {
    assert(str_arg.has_value());
    return *str_arg;
  }

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
#endif
