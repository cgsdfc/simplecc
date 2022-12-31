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

#ifndef SIMPLECC_LEX_TOKENIZE_H
#define SIMPLECC_LEX_TOKENIZE_H
#include "simplecc/Lex/TokenInfo.h"
#include <iostream>
#include <vector>

namespace simplecc {
/// This function tokenizes an input stream and adds all tokens to a vector.
void Tokenize(std::istream &Input, std::vector<TokenInfo> &Output);

/// This function prints a vector of tokens to an output stream with proper align.
void PrintTokens(const std::vector<TokenInfo> &Tokens, std::ostream &O);
} // namespace simplecc
#endif // SIMPLECC_LEX_TOKENIZE_H
