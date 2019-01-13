#ifndef SIMPLECC_PARSE_PARSE_H
#define SIMPLECC_PARSE_PARSE_H
#include "simplecc/Lex/TokenInfo.h"
#include "simplecc/Parse/AST.h"
#include "simplecc/Parse/Node.h"
#include <iostream>
#include <memory> // for unique_ptr
#include <string>

namespace simplecc {

/// Parse the tokens and create a parse tree (or concrete syntax tree) from them.
std::unique_ptr<Node>
BuildCST(const std::vector<TokenInfo> &TheTokens);

/// Parse the tokens and create an AST from them.
/// Return nullptr on error.
std::unique_ptr<ProgramAST, DeleteAST>
BuildAST(const std::string &Filename, const std::vector<TokenInfo> &TheTokens);

} // namespace simplecc
#endif // SIMPLECC_PARSE_PARSE_H