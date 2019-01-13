#include "simplecc/Lex/Tokenize.h"
#include <algorithm>
#include <iterator>
#ifdef _MSC_VER
#include <cctype>
#endif

using namespace simplecc;

/// Return if a char is valid to be in a string literal.
static inline bool IsValidStrChar(char Chr) {
  return Chr == 32 || Chr == 33 || (35 <= Chr && Chr <= 126);
}

/// Return if a char begins an identifier.
static inline bool IsNameBegin(char Chr) {
  return Chr == '_' || std::isalpha(Chr);
}

/// Return if a char can appear in the middle and end of an identifier.
static inline bool IsNameMiddle(char Chr) {
  return Chr == '_' || std::isalnum(Chr);
}

/// Return if a line consists totally space.
static bool IsBlank(const std::string &Line) {
  // Find the first char that is not space.
  return std::find_if_not(Line.begin(), Line.end(),
                          [](char C) { return std::isspace(C); }) == Line.end();
}

/// Return if a char is a valid one in a character literal.
static inline bool IsValidChar(char Chr) {
  static const std::string ValidChars("+-*/_");
  return ValidChars.find(Chr) != std::string::npos || std::isalnum(Chr);
}

/// Return if a char is _special_.
/// Brackets, parentheses, braces, semicolon, colon and comma are _special_.
static inline bool IsSpecial(char Chr) {
  static const std::string Special("[](){};:,");
  return Special.find(Chr) != std::string::npos;
}

/// Return if a char is (part of) an operator.
static inline bool IsOperator(char Chr) {
  static const std::string Operators("+-*/<>!=");
  return Operators.find(Chr) != std::string::npos;
}

/// Lower-case all chars in Str.
static inline void ToLowerInplace(std::string &Str) {
  std::transform(Str.begin(), Str.end(), Str.begin(), ::tolower);
}

namespace simplecc {
// TODO: make this a class.
void Tokenize(std::istream &Input, std::vector<TokenInfo> &Output) {
  unsigned Lineno = 0;
  std::string TheLine;

  Output.clear();
  while (std::getline(Input, TheLine)) {
    ++Lineno;
    if (IsBlank(TheLine))
      continue;

    unsigned Pos = 0;
    auto Max = TheLine.size();

    while (Pos < Max) {
      Location Start(Lineno, Pos);
      Symbol Type = Symbol::ERRORTOKEN;

      if (std::isdigit(TheLine[Pos])) {
        while (std::isdigit(TheLine[Pos])) {
          ++Pos;
        }
        Type = Symbol::NUMBER;
      } else if (TheLine[Pos] == '\'') {
        ++Pos;
        if (IsValidChar(TheLine[Pos])) {
          ++Pos;
          if (TheLine[Pos] == '\'') {
            ++Pos;
            Type = Symbol::CHAR;
          }
        }
      } else if (TheLine[Pos] == '\"') {
        ++Pos;
        while (IsValidStrChar(TheLine[Pos])) {
          ++Pos;
        }
        if (TheLine[Pos] == '\"') {
          ++Pos;
          Type = Symbol::STRING;
        }
      } else if (IsNameBegin(TheLine[Pos])) {
        ++Pos;
        while (IsNameMiddle(TheLine[Pos]))
          ++Pos;
        Type = Symbol::NAME;
      } else if (IsSpecial(TheLine[Pos])) {
        ++Pos;
        Type = Symbol::OP;
      } else if (IsOperator(TheLine[Pos])) {
        char chr = TheLine[Pos];
        ++Pos;
        if (chr == '>' || chr == '<' || chr == '=') {
          Type = Symbol::OP;
          if (TheLine[Pos] == '=') {
            ++Pos;
          }
        } else if (chr == '!') {
          if (TheLine[Pos] == '=') {
            ++Pos;
            Type = Symbol::OP;
          }
        } else {
          Type = Symbol::OP;
        }
      } else if (std::isspace(TheLine[Pos])) {
        while (std::isspace(TheLine[Pos]))
          ++Pos;
        continue;
      } else {
        ++Pos; // ERRORTOKEN
      }
      std::string Str(TheLine.begin() + Start.getColumn(),
                      TheLine.begin() + Pos);
      if (Type == Symbol::NAME) {
        ToLowerInplace(Str);
      }
      Output.emplace_back(Type, Str, Start, TheLine);
    }
  }
  Output.emplace_back(Symbol::ENDMARKER, "", Location(Lineno, 0), "");
}

void PrintTokens(const std::vector<TokenInfo> &Tokens, std::ostream &O) {
  std::copy(Tokens.begin(), Tokens.end(), std::ostream_iterator<TokenInfo>(O, "\n"));
}
} // namespace simplecc