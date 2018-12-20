#ifndef TOKENINFO_H
#define TOKENINFO_H
#include "simplecc/Parse/Grammar.h"

#include <iostream>
#include <string>

namespace simplecc {
using String = std::string;

bool IsTerminal(Symbol S);

inline bool IsNonTerminal(Symbol S) { return !IsTerminal(S); }

const char *getSymbolName(Symbol S);

class Location {
  unsigned Line;
  unsigned Column;

public:
  Location() : Line(0), Column(0) {}
  Location(unsigned lineno, unsigned col_offset)
      : Line(lineno), Column(col_offset) {}

  unsigned getLineNo() const { return Line; }

  unsigned getColOffset() const { return Column; }

  void Format(std::ostream &O) const;

  void FormatCompact(std::ostream &O) const;
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
