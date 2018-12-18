#ifndef TOKENIZE_H
#define TOKENIZE_H
#include "simplecc/Grammar.h"
#include "simplecc/TokenInfo.h"

#include <iostream>
#include <vector>

namespace simplecc {
void Tokenize(std::istream &Input, std::vector<TokenInfo> &Output);
void PrintTokens(const std::vector<TokenInfo> &Tokens, std::ostream &O);
} // namespace simplecc
#endif
