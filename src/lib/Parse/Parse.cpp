#include "simplecc/Parse/Parse.h"
#include "simplecc/Parse/Parser.h"
#include "simplecc/Parse/AstBuilder.h"

namespace simplecc {

std::unique_ptr<Node> BuildCST(const std::vector<TokenInfo> &TheTokens) {
  Parser P(&CompilerGrammar);
  return std::unique_ptr<Node>(P.ParseTokens(TheTokens));
}

std::unique_ptr<Program> BuildAST(const std::vector<TokenInfo> &TheTokens) {
  auto CST = BuildCST(TheTokens);
  if (!CST)
    return nullptr;
  return std::unique_ptr<Program>(AstBuilder().Build(CST.get()));
}

}