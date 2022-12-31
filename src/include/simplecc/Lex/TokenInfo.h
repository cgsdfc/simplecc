// MIT License
// 
// Copyright (c) 2018 Cong Feng.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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