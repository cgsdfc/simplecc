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

inline bool IsTerminal(int type) {
  return type < NT_OFFSET;
}

inline bool IsNonterminal(int type) {
  return !IsTerminal(type);
}

inline const char *GetSymName(Symbol s) {
  auto sym = static_cast<int>(s);
  return IsTerminal(sym) ? TokenNames[sym] : SymbolNames[sym - NT_OFFSET];
}

class Location {
public:
  unsigned lineno;
  unsigned col_offset;

  Location(unsigned lineno = -1, unsigned col_offset = -1):
    lineno(lineno), col_offset(col_offset) {}

  bool IsInvalid() const {
    return lineno == -1 || col_offset == -1;
  }

  std::string ToString() const {
    if (IsInvalid())
      return "None";

    std::ostringstream os;
    os << lineno << ',' << col_offset;
    return os.str();
  }

  void Format(std::ostream &os) const {
    if (IsInvalid()) {
      os << "None";
    } else {
      os << "(" << lineno << ", " << col_offset << ")";
    }
  }

};

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
