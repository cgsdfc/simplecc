#ifndef SIMPLECC_TARGET_MIPSSUPPORT_H
#define SIMPLECC_TARGET_MIPSSUPPORT_H
#include "simplecc/Support/Macros.h"
#include <iostream>
#include <string>

namespace simplecc {

// Return the bytes for n entries
inline constexpr int BytesFromEntries(int NumEntry) { return 4 * NumEntry; }

enum class MipsSyscallCode : unsigned {
  PRINT_STRING = 4,
  PRINT_CHARACTER = 11,
  PRINT_INTEGER = 1,
  READ_INTEGER = 5,
  READ_CHARACTER = 12,
  EXIT_PROGRAM = 10,
};

inline std::ostream &operator<<(std::ostream &O, MipsSyscallCode syscall) {
  return O << static_cast<unsigned>(syscall);
}

/// CRTP base for label.
template <typename Derived> class LabelBase {
protected:
  /// Whether this label needs a colon at the end.
  bool NeedColon;

public:
  explicit LabelBase(bool N) : NeedColon(N) {}
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
  void FormatImpl(std::ostream &O) const;
};

class JumpTargetLabel : public LabelBase<JumpTargetLabel> {
  const char *ParentName;
  unsigned Target;

public:
  JumpTargetLabel(const char *PN, unsigned T, bool NeedColon)
      : LabelBase(NeedColon), ParentName(PN), Target(T) {}
  JumpTargetLabel(const std::string &PN, unsigned T, bool NeedColon)
      : JumpTargetLabel(PN.data(), T, NeedColon) {}

  void FormatImpl(std::ostream &O) const;
};

class ReturnLabel : public LabelBase<ReturnLabel> {
  const char *ParentName;

public:
  ReturnLabel(const std::string &PN, bool NeedColon)
      : LabelBase(NeedColon), ParentName(PN.data()) {}
  void FormatImpl(std::ostream &O) const;
};

/// A Global label that is unique with the module.
class GlobalLabel : public LabelBase<GlobalLabel> {
  const char *Name;

public:
  GlobalLabel(const char *N, bool NeedColon) : LabelBase(NeedColon), Name(N) {}
  GlobalLabel(const std::string &N, bool NeedColon)
      : GlobalLabel(N.data(), NeedColon) {}

  void FormatImpl(std::ostream &O) const;
};

/// A wrapper class that escapes the string when Format'ted.
class EscapedString {
  const std::string &Str;

public:
  EscapedString(const std::string &S) : Str(S) {}
  void Format(std::ostream &O) const;
};

DEFINE_INLINE_OUTPUT_OPERATOR(EscapedString)

} // namespace simplecc
#endif