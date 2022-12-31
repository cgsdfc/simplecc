// MIT License
// 
// Copyright (c) 2018 Cong Feng.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "simplecc/Parse/Parse.h"
#include "simplecc/Parse/ASTBuilder.h"
#include "simplecc/Parse/Parser.h"

namespace simplecc {
std::unique_ptr<Node> BuildCST(const std::vector<TokenInfo> &TheTokens) {
  Parser P(&CompilerGrammar);
  return P.ParseTokens(TheTokens);
}

std::unique_ptr<ProgramAST, DeleteAST>
BuildAST(const std::string &Filename, const std::vector<TokenInfo> &TheTokens) {
  auto CST = BuildCST(TheTokens);
  if (!CST)
    return nullptr;
  return ASTBuilder().Build(Filename, CST.get());
}

} // namespace simplecc