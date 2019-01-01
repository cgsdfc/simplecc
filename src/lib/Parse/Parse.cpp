#include "simplecc/Parse/Parse.h"
#include "simplecc/Parse/ASTBuilder.h"
#include "simplecc/Parse/Parser.h"

namespace simplecc {

std::unique_ptr<Node> BuildCST(const std::vector<TokenInfo> &TheTokens) {
  Parser P(&CompilerGrammar);
  return std::unique_ptr<Node>(P.ParseTokens(TheTokens));
}

ProgramRef BuildAST(const std::string &Filename, const std::vector<TokenInfo> &TheTokens) {
  auto CST = BuildCST(TheTokens);
  if (!CST)
    return nullptr;
  return ProgramRef(ASTBuilder().Build(Filename, CST.get()));
}

} // namespace simplecc