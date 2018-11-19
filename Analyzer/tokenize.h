#ifndef TOKENIZE_H
#define TOKENIZE_H

#include "Grammar.h"

#include <fstream>
#include <sstream>
#include <cassert>
#include <cctype>
#include <iostream>
#include <vector>
#include <cstdio>

typedef std::string String;
using std::fprintf;
using std::fputs;
using std::exit;

inline bool IsTerminal(Symbol type) {
  return static_cast<int>(type) < NT_OFFSET;
}

inline bool IsNonterminal(Symbol type) {
  return !IsTerminal(type);
}

inline const char *GetSymName(Symbol s) {
  auto sym = static_cast<int>(s);
  return IsTerminal(s) ? TokenNames[sym] : SymbolNames[sym - NT_OFFSET];
}

class Location {
public:
  unsigned lineno;
  unsigned col_offset;

  Location(): lineno(0), col_offset(0) {}
  Location(unsigned lineno, unsigned col_offset):
    lineno(lineno), col_offset(col_offset) {}

  std::string ToString() const {
    std::ostringstream os;
    os << lineno << ',' << col_offset;
    return os.str();
  }

  void Format(std::ostream &os) const {
    os << "(" << lineno << ", " << col_offset << ")";
  }

};

inline bool operator==(const Location &l1, const Location &l2) {
  return l1.lineno == l2.lineno && l1.col_offset == l2.col_offset;
}

inline std::ostream &operator<<(std::ostream &os, const Location &loc) {
  loc.Format(os);
  return os;
}

class TokenInfo {
public:
  Symbol type;
  String string;
  Location start;
  Location end;
  String line;

  TokenInfo(Symbol type, const String &string, Location start, Location end,
      String line): type(type), string(string), start(start), end(end), line(line) {}

  void Format(std::FILE *out) const {
    std::ostringstream os;
    os << start.ToString() << '-' << end.ToString() << ':';

    std::fprintf(out, "%-20s%-15s%-15s\n",
        os.str().c_str(),
        GetSymName(type),
        string.c_str());
  }
};

typedef std::vector<TokenInfo*> TokenBuffer;

void Tokenize(std::istream &Input, TokenBuffer &Output);

#endif
