#ifndef SIMPLECC_LEX_TOKENINFO_H
#define SIMPLECC_LEX_TOKENINFO_H
#include "simplecc/Lex/Location.h"
#include "simplecc/Parse/Grammar.h"
#include "simplecc/Support/Macros.h"
#include <iostream>
#include <string>
#include <utility>

namespace simplecc {
/// This class represents a single token.
class TokenInfo {
public:
  TokenInfo(Symbol Ty, std::string S, Location Loc, std::string Line);
  TokenInfo(const TokenInfo &) = default;
  TokenInfo(TokenInfo &&) = default;

  /// Return the name of the Symbol.
  const char *getTypeName() const;

  /// Return the location this token was found.
  Location getLocation() const { return Loc; }

  /// Return the string value of this token.
  /// Virtual tokens like ENDMARKER have an empty one.
  const std::string &getString() const { return Str; }

  /// Return the line of code where this token was found.
  const std::string &getLine() const { return Line; }

  /// Return the type of this token, which must be a terminal.
  Symbol getType() const { return Type; }

  /// Format each field of the token aligned properly.
  void Format(std::ostream &O) const;

  /// Return if a Symbol is a non-terminal.
  static constexpr bool IsNonTerminal(Symbol S) {
    return !IsTerminal(S);
  }
  /// Return if a Symbol is a terminal.
  static constexpr bool IsTerminal(Symbol S) {
    return static_cast<int>(S) < NT_OFFSET;
  }
  /// Return the name for a Symbol.
  static const char *getSymbolName(Symbol S);

private:
  Symbol Type;
  std::string Str;
  Location Loc;
  std::string Line;
};

DEFINE_INLINE_OUTPUT_OPERATOR(TokenInfo)
} // namespace simplecc
#endif // SIMPLECC_LEX_TOKENINFO_H