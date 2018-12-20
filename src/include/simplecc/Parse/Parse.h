#ifndef SIMPLECOMPILER_PARSE_H
#define SIMPLECOMPILER_PARSE_H
#include <memory> // for unique_ptr
#include "Node.h"
#include "AST.h"

namespace simplecc {
std::unique_ptr<Node> BuildCST(const std::vector<TokenInfo> &TheTokens);
std::unique_ptr<Program> BuildAST(const std::vector<TokenInfo> &TheTokens);
}
#endif //SIMPLECOMPILER_PARSE_H
