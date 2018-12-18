#include "simplecc/Tokenize.h"
#include "simplecc/ErrorManager.h"

#include <cctype>
#include <algorithm>
#include <iomanip>
#include <string> // getline()

using namespace simplecc;

static bool IsBlank(const String &Line) {
  for (auto Chr : Line)
    if (!std::isspace(Chr))
      return false;
  return true;
}

static inline bool IsValidChar(char Chr) {
  static const String ValidChars("+-*/_");
  return ValidChars.find(Chr) != String::npos || std::isalnum(Chr);
}

static inline bool IsValidStrChar(char Chr) {
  return Chr == 32 || Chr == 33 || (35 <= Chr && Chr <= 126);
}

static inline bool IsNameBegin(char Chr) { return Chr == '_' || std::isalpha(Chr); }

static inline bool IsNameMiddle(char Chr) { return Chr == '_' || std::isalnum(Chr); }

static inline bool IsSpecial(char Chr) {
  static String Special("[](){};:,");
  return Special.find(Chr) != String::npos;
}

static inline bool IsOperator(char Chr) {
  static String Operators("+-*/<>!=");
  return Operators.find(Chr) != String::npos;
}

static inline void ToLowerInplace(String &Str) {
  std::transform(Str.begin(), Str.end(), Str.begin(), ::tolower);
}


namespace simplecc {
void Tokenize(std::istream &Input, std::vector<TokenInfo> &Output) {
  unsigned Lineno = 0;
  String TheLine;

  Output.clear();
  while (std::getline(Input, TheLine)) {
    ++Lineno;
    if (IsBlank(TheLine))
      continue;

    unsigned Pos = 0;
    unsigned Max = TheLine.size();

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
      String Str(TheLine.begin() + Start.getColOffset(), TheLine.begin() + Pos);
      if (Type == Symbol::NAME) {
        ToLowerInplace(Str);
      }
      Output.emplace_back(Type, Str, Start, TheLine);
    }
  }
  Output.emplace_back(Symbol::ENDMARKER, "", Location(Lineno, 0), "");
}

static void DumpTokenInfo(std::ostream &O, const TokenInfo &T) {
  auto &&Loc = T.getLocation();
  std::ostringstream OS;
  OS << Loc.getLineNo() << ',' << Loc.getColOffset() << ":";
  auto &&Range = OS.str();

  O << std::left << std::setw(20) << Range;
  O << std::left << std::setw(15) << T.getTypeName();
  O << std::left << std::setw(15) << Quote(T.getString()) << "\n";
}

void PrintTokens(const std::vector<TokenInfo> &Tokens, std::ostream &O) {
  std::for_each(Tokens.begin(), Tokens.end(), [&O](const TokenInfo &T) {
    DumpTokenInfo(O, T);
  });
}

} // namespace simplecc

void TokenInfo::Format(std::ostream &O) const {
  O << "TokenInfo("
     << "type=" << getTypeName() << ", "
     << "string=" << Quote(getString()) << ", "
     << getLocation() << ", "
     << "line=" << Quote(getLine()) << ")";
}

const char *TokenInfo::getTypeName() const { return getSymbolName(Type); }
