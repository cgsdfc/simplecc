#include "tokenize.h"

#include <algorithm>

bool IsBlank(const String &line) {
  for (auto ch: line)
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

bool IsNameBegin(char ch) {
  return ch == '_' || std::isalpha(ch);
}

bool IsNameMiddle(char ch) {
  return ch == '_' || std::isalnum(ch);
}

bool IsSpecial(char ch) {
  static String special("[](){};:,");
  return special.find(ch) != String::npos;
}

bool IsOperator(char ch) {
  static String operators("+-*/<>!=");
  return operators.find(ch) != String::npos;
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
      }
      else if (line[pos] == '\'') {
        ++pos;
        if (IsValidChar(line[pos])) {
          ++pos;
          if (line[pos] == '\'') {
            ++pos;
            type = Symbol::CHAR;
          }
        }
      }
      else if (line[pos] == '\"') {
        ++pos;
        while (IsValidStrChar(line[pos])) {
          ++pos;
        }
        if (line[pos] == '\"') {
            ++pos;
            type = Symbol::STRING;
          }
      }
      else if (IsNameBegin(line[pos])) {
        ++pos;
        while (IsNameMiddle(line[pos]))
          ++pos;
        type = Symbol::NAME;
      }
      else if (IsSpecial(line[pos])) {
        ++pos;
        type = Symbol::OP;
      }
      else if (IsOperator(line[pos])) {
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
      }
      else if (std::isspace(line[pos])) {
        while (std::isspace(line[pos]))
          ++pos;
        continue;
      }
      else {
        ++pos; // ERRORTOKEN
      }
      Location end(lnum, pos);
      String token(line.begin() + start.col_offset, line.begin() + end.col_offset);
      if (type == Symbol::NAME) {
        std::transform(token.begin(), token.end(), token.begin(), ::tolower);
      }
      Output.push_back(new TokenInfo(type, token, start, end, line));
    }
  }
  Output.push_back(new TokenInfo(Symbol::ENDMARKER, "", Location(lnum, 0), Location(lnum, 0), ""));
}
