#ifndef TOKENIZE_H
#define TOKENIZE_H
#include "Grammar.h"
#include "TokenInfo.h"

#include <iostream>
#include <vector>

namespace simplecompiler {
void Tokenize(std::istream &Input, std::vector<TokenInfo> &Output);
void PrintTokens(const std::vector<TokenInfo> &tokens, std::ostream &os);
} // namespace simplecompiler
#endif
