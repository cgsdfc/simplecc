#ifndef SIMPLECC_PARSE_PARSE_H
#define SIMPLECC_PARSE_PARSE_H
#include "simplecc/Lex/TokenInfo.h"
#include "simplecc/Parse/AST.h"
#include "simplecc/Parse/Node.h"
#include <iostream>
#include <memory> // for unique_ptr
#include <string>

namespace simplecc {
using ProgramRef = std::unique_ptr<Program, DeleteAST>;

std::unique_ptr<Node> BuildCST(const std::vector<TokenInfo> &TheTokens);
ProgramRef BuildAST(const std::string &Filename,
                    const std::vector<TokenInfo> &TheTokens);
} // namespace simplecc
#endif // SIMPLECC_PARSE_PARSE_H
