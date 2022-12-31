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

#ifndef SIMPLECC_PARSE_GRAMMAR_H
#define SIMPLECC_PARSE_GRAMMAR_H
#include "simplecc/Parse/GramDef.h"

namespace simplecc {
enum class Symbol {
  program = 256,
  arglist = 257,
  atom = 258,
  atom_trailer = 259,
  compound_stmt = 260,
  condition = 261,
  const_decl = 262,
  const_item = 263,
  decl_trailer = 264,
  declaration = 265,
  expr = 266,
  factor = 267,
  flow_stmt = 268,
  for_stmt = 269,
  if_stmt = 270,
  integer = 271,
  paralist = 272,
  read_stmt = 273,
  return_stmt = 274,
  stmt = 275,
  stmt_trailer = 276,
  subscript2 = 277,
  term = 278,
  type_name = 279,
  var_decl = 280,
  var_item = 281,
  while_stmt = 282,
  write_stmt = 283,
  NAME = 0,
  OP = 1,
  ERRORTOKEN = 2,
  ENDMARKER = 3,
  CHAR = 4,
  NUMBER = 5,
  STRING = 6,
};

constexpr const unsigned NT_OFFSET = 256;
extern Grammar CompilerGrammar;
extern const char *TokenNames[], *SymbolNames[];
} // namespace simplecc
#endif // SIMPLECC_PARSE_GRAMMAR_H