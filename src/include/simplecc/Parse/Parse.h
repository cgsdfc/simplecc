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

#ifndef SIMPLECC_PARSE_PARSE_H
#define SIMPLECC_PARSE_PARSE_H
#include "simplecc/Lex/TokenInfo.h"
#include "simplecc/AST/AST.h"
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