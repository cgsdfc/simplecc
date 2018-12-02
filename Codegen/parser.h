#ifndef PARSER_H
#define PARSER_H

#include "tokenize.h"

namespace simplecompiler {
class Node;
Node *ParseTokens(const TokenBuffer &tokens);
} // namespace simplecompiler
#endif
