#ifndef SIMPLECC_LEX_TOKENINFO_H
#define SIMPLECC_LEX_TOKENINFO_H
#include "simplecc/Parse/Grammar.h"
#include "simplecc/Lex/Location.h"

#include <iostream>
#include <string>
#include <utility>

namespace simplecc {
using String = std::string;

bool IsTerminal(Symbol S);

inline bool IsNonTerminal(Symbol S) { return !IsTerminal(S); }

const char *getSymbolName(Symbol S);

class TokenInfo {
public:
  TokenInfo(Symbol Ty, String S, const Location &Loc, String Line)
      : Type(Ty), Str(std::move(S)), Loc(Loc), Line(std::move(Line)) {}

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
