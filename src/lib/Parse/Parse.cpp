#include "simplecc/Parse/Parse.h"
#include "simplecc/Parse/AstBuilder.h"
#include "simplecc/Parse/Parser.h"

namespace simplecc {

std::unique_ptr<Node> BuildCST(const std::vector<TokenInfo> &TheTokens) {
  Parser P(&CompilerGrammar);
  return std::unique_ptr<Node>(P.ParseTokens(TheTokens));
}

ProgramRef BuildAST(const std::vector<TokenInfo> &TheTokens) {
  auto CST = BuildCST(TheTokens);
  if (!CST)
    return nullptr;
  return ProgramRef(AstBuilder().Build(CST.get()));
}

} // namespace simplecc