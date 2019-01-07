#ifndef SIMPLECC_LEX_TOKENINFO_H
#define SIMPLECC_LEX_TOKENINFO_H
#include "simplecc/Lex/Location.h"
#include "simplecc/Parse/Grammar.h"
#include <iostream>
#include <string>
#include <utility>

namespace simplecc {
bool IsTerminal(Symbol S);
inline bool IsNonTerminal(Symbol S) { return !IsTerminal(S); }
const char *getSymbolName(Symbol S);

class TokenInfo {
public:
  TokenInfo(Symbol Ty, std::string S, Location Loc, std::string Line)
      : Type(Ty), Str(std::move(S)), Loc(Loc), Line(std::move(Line)) {}

  TokenInfo(const TokenInfo &) = default;
  TokenInfo(TokenInfo &&) = default;

  const char *getTypeName() const;
  Location getLocation() const { return Loc; }
  const std::string &getString() const { return Str; }
  const std::string &getLine() const { return Line; }
  Symbol getType() const { return Type; }

  void Format(std::ostream &O) const;

private:
  Symbol Type;
  std::string Str;
  Location Loc;
  std::string Line;
};

} // namespace simplecc
#endif