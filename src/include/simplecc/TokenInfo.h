#ifndef TOKENINFO_H
#define TOKENINFO_H
#include "simplecc/Grammar.h"

#include <iostream>
#include <string>

namespace simplecc {
using String = std::string;

inline bool IsTerminal(Symbol type) {
  return static_cast<int>(type) < NT_OFFSET;
}

inline bool IsNonterminal(Symbol type) { return !IsTerminal(type); }

inline const char *GetSymbolName(Symbol s) {
  auto sym = static_cast<int>(s);
  return IsTerminal(s) ? TokenNames[sym] : SymbolNames[sym - NT_OFFSET];
}

class Location {
  unsigned lineno;
  unsigned col_offset;

public:
  Location() : lineno(0), col_offset(0) {}
  Location(unsigned lineno, unsigned col_offset)
      : lineno(lineno), col_offset(col_offset) {}

  unsigned getLineNo() const { return lineno; }
  unsigned getColOffset() const { return col_offset; }

  void Format(std::ostream &os) const {
    os << "Location(" << lineno << ", " << col_offset << ")";
  }

  void FormatCompact(std::ostream &os) const {
    os << lineno << ":" << col_offset << ":";
  }
};

inline std::ostream &operator<<(std::ostream &os, const Location &loc) {
  loc.Format(os);
  return os;
}

class TokenInfo {
  Symbol type;
  String string;
  Location start;
  String line;

public:
  TokenInfo(Symbol type, const String &string, const Location &start,
            const String &line)
      : type(type), string(string), start(start), line(line) {}

  TokenInfo(const TokenInfo &) = default;
  TokenInfo(TokenInfo &&) = default;

  void Format(std::ostream &os) const;
  const char *GetTypeName() const;
  const Location &getLocation() const { return start; }
  const String &getString() const { return string; }
  const String &getLine() const { return line; }
  Symbol getType() const { return type; }
};

} // namespace simplecc
#endif
