#ifndef TOKENIZE_H
#define TOKENIZE_H

#include <fstream>
#include <sstream>
#include <cassert>
#include <cctype>
#include <iostream>
#include <vector>
#include <cstdio>

#include "token.h"

typedef std::string String;

class Location {
public:
  unsigned lineno;
  unsigned col_offset;

  Location(unsigned lineno, unsigned col_offset):
    lineno(lineno), col_offset(col_offset) {}

  std::string ToString() const {
    std::ostringstream os;
    os << lineno << ',' << col_offset;
    return os.str();
  }

};

class TokenInfo {
public:
  unsigned type;
  String string;
  Location start;
  Location end;
  String line;

  TokenInfo(unsigned type, const String &string, Location start, Location end,
      String line): type(type), string(string), start(start), end(end) {}

  void Format(std::FILE *out) const {
    std::ostringstream os;
    os << start.ToString() << '-' << end.ToString() << ':';

    std::fprintf(out, "%-20s%-15s%-15s\n",
        os.str().c_str(),
        TokenNames[type],
        string.c_str());
  }
};

typedef std::vector<TokenInfo*> TokenBuffer;

void Tokenize(std::istream &Input, TokenBuffer &Output);

#endif
