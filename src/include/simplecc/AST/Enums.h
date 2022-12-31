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

#ifndef SIMPLECC_AST_ENUMS_H
#define SIMPLECC_AST_ENUMS_H
#include <iostream>
#include <string>

namespace simplecc {
enum class BinaryOpKind {
#define HANDLE_OPERATOR(Val, Str, FUNC) Val,
#include "Enums.def"
};

enum class UnaryOpKind {
#define HANDLE_UNARYOP(Val, Str) Val,
#include "Enums.def"
};

enum class ExprContextKind {
#define HANDLE_EXPRCONTEXT(Val, Str) Val,
#include "Enums.def"
};

enum class BasicTypeKind {
#define HANDLE_BASICTYPE(Val, Str) Val,
#include "Enums.def"
};

std::ostream &operator<<(std::ostream &os, BinaryOpKind val);
std::ostream &operator<<(std::ostream &os, UnaryOpKind val);
std::ostream &operator<<(std::ostream &os, ExprContextKind val);
std::ostream &operator<<(std::ostream &os, BasicTypeKind val);

BinaryOpKind OperatorKindFromString(const std::string &s);
const char *CStringFromOperatorKind(BinaryOpKind val);

UnaryOpKind UnaryopKindFromString(const std::string &s);
const char *CStringFromUnaryopKind(UnaryOpKind val);

BasicTypeKind BasicTypeKindFromString(const std::string &s);
const char *CStringFromBasicTypeKind(BasicTypeKind val);
} // namespace simplecc
#endif // SIMPLECC_AST_ENUMS_H