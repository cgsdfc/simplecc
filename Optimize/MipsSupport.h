#ifndef MIPS_SUPPORT_H
#define MIPS_SUPPORT_H
#include <iostream>
#include <string>

namespace simplecompiler {
using String = std::string;

// Return the bytes for n entries
inline constexpr int BytesFromEntries(int n_entries) { return 4 * n_entries; }

enum class MipsSyscallNumber : unsigned {
  PRINT_STRING = 4,
  PRINT_CHARACTER = 11,
  PRINT_INTEGER = 1,
  READ_INTEGER = 5,
  READ_CHARACTER = 12,
  EXIT_PROGRAM = 10,
};

inline std::ostream &operator<<(std::ostream &os, MipsSyscallNumber syscall) {
  return os << static_cast<unsigned>(syscall);
}

/// CRTP base for label.
template <typename Derived> class LabelBase {
protected:
  /// Whether this label needs a colon at the end.
  bool NeedColon;

public:
  LabelBase(bool N) : NeedColon(N) {}
  void Format(std::ostream &O) const {
    static_cast<const Derived *>(this)->FormatImpl(O);
    if (NeedColon)
      O << ":";
  }
};

template <typename Derived>
inline std::ostream &operator<<(std::ostream &O, const LabelBase<Derived> &LB) {
  LB.Format(O);
  return O;
}

class AsciizLabel : public LabelBase<AsciizLabel> {
  unsigned StringID;

public:
  AsciizLabel(unsigned N, bool NeedColon) : LabelBase(NeedColon), StringID(N) {}
  void FormatImpl(std::ostream &O) const { O << "string_" << StringID; }
};

class JumpTargetLabel : public LabelBase<JumpTargetLabel> {
  const char *ParentName;
  unsigned Target;

public:
  JumpTargetLabel(const char *PN, unsigned T, bool NeedColon)
      : LabelBase(NeedColon), ParentName(PN), Target(T) {}
  JumpTargetLabel(const String &PN, unsigned T, bool NeedColon)
      : JumpTargetLabel(PN.data(), T, NeedColon) {}

  void FormatImpl(std::ostream &O) const {
    O << ParentName << "_label_" << Target;
  }
};

class ReturnLabel : public LabelBase<ReturnLabel> {
  const char *ParentName;

public:
  ReturnLabel(const String &PN, bool NeedColon)
      : LabelBase(NeedColon), ParentName(PN.data()) {}
  void FormatImpl(std::ostream &O) const { O << ParentName << "_return"; }
};

/// A Global label that is unique with the module.
class GlobalLabel : public LabelBase<GlobalLabel> {
  const char *Name;

public:
  GlobalLabel(const char *N, bool NeedColon) : LabelBase(NeedColon), Name(N) {}
  GlobalLabel(const String &N, bool NeedColon)
      : GlobalLabel(N.data(), NeedColon) {}

  void FormatImpl(std::ostream &O) const { O << Name; }
};

/// A wrapper class that escapes the string when Format'ted.
class EscapedString {
  const String &Str;

public:
  EscapedString(const String &S) : Str(S) {}
  void Format(std::ostream &O) const;
};

inline std::ostream &operator<<(std::ostream &O, const EscapedString &E) {
  E.Format(O);
  return O;
}

} // namespace simplecompiler
#endif
