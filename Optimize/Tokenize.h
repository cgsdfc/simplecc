#ifndef TOKENIZE_H
#define TOKENIZE_H

#include "TokenInfo.h"
#include "Grammar.h"

#include <iostream>
#include <vector>

namespace simplecompiler {

using TokenBuffer = std::vector<TokenInfo>;

void Tokenize(std::istream &Input, TokenBuffer &Output);
void PrintTokens(const TokenBuffer &tokens, std::ostream &os);

} // namespace simplecompiler
#endif
