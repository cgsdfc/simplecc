#ifndef PARSER_H
#define PARSER_H

#include "TokenInfo.h"
#include <vector>

namespace simplecompiler {
class Node;
Node *ParseTokens(const std::vector<TokenInfo> &tokens);
} // namespace simplecompiler
#endif
