#include "tokenize.h"
#include "error.h"

#include <algorithm>
#include <cassert>
#include <sstream>

// Location(lineno, col_offset) => "lineno,col_offset"
String LocationToString(const Location &loc) {
  std::ostringstream os;
  os << loc.lineno << ',' << loc.col_offset;
  return os.str();
}

String LocationRangeToString(const Location &start) {
  std::ostringstream os;
  os << LocationToString(start) << ':';
  return os.str();
}

void DumpTokenInfo(std::FILE *out, const TokenInfo &token) {
  String &&token_range = LocationRangeToString(token.start);

  std::fprintf(out, "%-20s%-15s%-15s\n", token_range.c_str(),
               GetSymName(token.type), token.string.c_str());
}

void TokenInfo::Format(std::ostream &os) const {
  os << "TokenInfo("
     << "type=" << GetSymName(type) << ", "
     << "string=" << Quote(string) << ", "
     << "start=" << start << ", "
     << "line=" << Quote(line) << ")";
}

bool IsBlank(const String &line) {
  for (auto ch : line)
    if (!std::isspace(ch))
      return false;
  return true;
}

bool IsValidChar(char ch) {
  static const String valid_chars("+-*/_");
  return valid_chars.find(ch) != String::npos || std::isalnum(ch);
}

bool IsValidStrChar(char ch) {
  return ch == 32 || ch == 33 || (35 <= ch && ch <= 126);
}

bool IsNameBegin(char ch) { return ch == '_' || std::isalpha(ch); }

bool IsNameMiddle(char ch) { return ch == '_' || std::isalnum(ch); }

bool IsSpecial(char ch) {
  static String special("[](){};:,");
  return special.find(ch) != String::npos;
}

bool IsOperator(char ch) {
  static String operators("+-*/<>!=");
  return operators.find(ch) != String::npos;
}

void ToLowerInplace(String &string) {
  std::transform(string.begin(), string.end(), string.begin(), ::tolower);
}

void Tokenize(std::istream &Input, TokenBuffer &Output) {
  assert(Output.empty());
  unsigned lnum = 0;
  String line;

  while (std::getline(Input, line)) {
    ++lnum;
    if (IsBlank(line))
      continue;

    unsigned pos = 0;
    unsigned max = line.size();

    while (pos < max) {
      Location start(lnum, pos);
      Symbol type = Symbol::ERRORTOKEN;

      if (std::isdigit(line[pos])) {
        while (std::isdigit(line[pos])) {
          ++pos;
        }
        type = Symbol::NUMBER;
      } else if (line[pos] == '\'') {
        ++pos;
        if (IsValidChar(line[pos])) {
          ++pos;
          if (line[pos] == '\'') {
            ++pos;
            type = Symbol::CHAR;
          }
        }
      } else if (line[pos] == '\"') {
        ++pos;
        while (IsValidStrChar(line[pos])) {
          ++pos;
        }
        if (line[pos] == '\"') {
          ++pos;
          type = Symbol::STRING;
        }
      } else if (IsNameBegin(line[pos])) {
        ++pos;
        while (IsNameMiddle(line[pos]))
          ++pos;
        type = Symbol::NAME;
      } else if (IsSpecial(line[pos])) {
        ++pos;
        type = Symbol::OP;
      } else if (IsOperator(line[pos])) {
        char chr = line[pos];
        ++pos;
        if (chr == '>' || chr == '<' || chr == '=') {
          type = Symbol::OP;
          if (line[pos] == '=') {
            ++pos;
          }
        } else if (chr == '!') {
          if (line[pos] == '=') {
            ++pos;
            type = Symbol::OP;
          }
        } else {
          type = Symbol::OP;
        }
      } else if (std::isspace(line[pos])) {
        while (std::isspace(line[pos]))
          ++pos;
        continue;
      } else {
        ++pos; // ERRORTOKEN
      }
      String token(line.begin() + start.col_offset, line.begin() + pos);
      if (type == Symbol::NAME) {
        ToLowerInplace(token);
      }
      Output.emplace_back(type, token, start, line);
    }
  }
  Output.emplace_back(Symbol::ENDMARKER, "", Location(lnum, 0), "");
}
