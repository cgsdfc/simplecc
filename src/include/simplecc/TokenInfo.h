#ifndef TOKENINFO_H
#define TOKENINFO_H
#include "simplecc/Grammar.h"

#include <iostream>
#include <string>

namespace simplecc {
using String = std::string;

inline bool IsTerminal(Symbol S) {
  return static_cast<int>(S) < NT_OFFSET;
}

inline bool IsNonterminal(Symbol S) { return !IsTerminal(S); }

inline const char *getSymbolName(Symbol S) {
  auto Val = static_cast<int>(S);
  return IsTerminal(S) ? TokenNames[Val] : SymbolNames[Val - NT_OFFSET];
}

class Location {
  unsigned Line;
  unsigned Column;

public:
  Location() : Line(0), Column(0) {}
  Location(unsigned lineno, unsigned col_offset)
      : Line(lineno), Column(col_offset) {}

  unsigned getLineNo() const { return Line; }

  unsigned getColOffset() const { return Column; }

  void Format(std::ostream &O) const {
    O << "Location(" << Line << ", " << Column << ")";
  }

  void FormatCompact(std::ostream &O) const {
    O << Line << ":" << Column << ":";
  }
};

inline std::ostream &operator<<(std::ostream &os, const Location &loc) {
  loc.Format(os);
  return os;
}

class TokenInfo {
public:
  TokenInfo(Symbol Ty, const String &S, const Location &Loc,
            const String &Line)
      : Type(Ty), Str(S), Loc(Loc), Line(Line) {}

  TokenInfo(const TokenInfo &) = default;
  TokenInfo(TokenInfo &&) = default;

  const char *getTypeName() const;

  const Location &getLocation() const { return Loc; }

  const String &getString() const { return Str; }

  const String &getLine() const { return Line; }

  Symbol getType() const { return Type; }

  void Format(std::ostream &O) const;

private:
  Symbol Type;
  String Str;
  Location Loc;
  String Line;
};

} // namespace simplecc
#endif
