#ifndef PARSER_H
#define PARSER_H

#include "simplecc/TokenInfo.h"
#include <vector>

namespace simplecc {
class Node;
Node *ParseTokens(const std::vector<TokenInfo> &tokens);
} // namespace simplecc
#endif
