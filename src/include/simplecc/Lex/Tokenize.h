#ifndef SIMPLECC_LEX_TOKENIZE_H
#define SIMPLECC_LEX_TOKENIZE_H
#include "simplecc/Lex/TokenInfo.h"
#include <iostream>
#include <vector>

namespace simplecc {
/// This function tokenizes an input stream and adds all tokens to a vector.
void Tokenize(std::istream &Input, std::vector<TokenInfo> &Output);

/// This function prints a vector of tokens to an output stream with proper align.
void PrintTokens(const std::vector<TokenInfo> &Tokens, std::ostream &O);
} // namespace simplecc
#endif // SIMPLECC_LEX_TOKENIZE_H
