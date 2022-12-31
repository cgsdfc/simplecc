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

#include "simplecc/AST/Enums.h"
#include <cassert>

namespace simplecc {

std::ostream &operator<<(std::ostream &os, BinaryOpKind val) {
  switch (val) {
  default:
    assert(false && "Invalid Enum Value");
#define HANDLE_OPERATOR(VAL, STR, FUNC)                                        \
  case BinaryOpKind::VAL:                                                      \
    return os << #VAL;
#include "simplecc/AST/Enums.def"
  }
}

std::ostream &operator<<(std::ostream &os, UnaryOpKind val) {
  switch (val) {
  default:
    assert(false && "Invalid Enum Value");
#define HANDLE_UNARYOP(VAL, STR)                                               \
  case UnaryOpKind::VAL:                                                       \
    return os << #VAL;
#include "simplecc/AST/Enums.def"
  }
}

std::ostream &operator<<(std::ostream &os, ExprContextKind val) {
  switch (val) {
  default:
    assert(false && "Invalid Enum Value");
#define HANDLE_EXPRCONTEXT(VAL, STR)                                           \
  case ExprContextKind::VAL:                                                   \
    return os << #VAL;
#include "simplecc/AST/Enums.def"
  }
}

std::ostream &operator<<(std::ostream &os, BasicTypeKind val) {
  switch (val) {
  default:
    assert(false && "Invalid Enum Value");
#define HANDLE_BASICTYPE(VAL, STR)                                             \
  case BasicTypeKind::VAL:                                                     \
    return os << #VAL;
#include "simplecc/AST/Enums.def"
  }
}

BinaryOpKind OperatorKindFromString(const std::string &s) {
#define HANDLE_OPERATOR(Val, Str, FUNC)                                        \
  if (s == Str)                                                                \
    return BinaryOpKind::Val;
#include "simplecc/AST/Enums.def"
  assert(false && "Invalid String Conversion");
}

const char *CStringFromOperatorKind(BinaryOpKind val) {
  switch (val) {
  default:
    assert(false && "Invalid Enum Value");
#define HANDLE_OPERATOR(Val, Str, FUNC)                                        \
  case BinaryOpKind::Val:                                                      \
    return Str;
#include "simplecc/AST/Enums.def"
  }
}

UnaryOpKind UnaryopKindFromString(const std::string &s) {
#define HANDLE_UNARYOP(Val, Str)                                               \
  if (s == Str)                                                                \
    return UnaryOpKind::Val;
#include "simplecc/AST/Enums.def"
  assert(false && "Invalid String Conversion");
}

const char *CStringFromUnaryopKind(UnaryOpKind val) {
  switch (val) {
  default:
    assert(false && "Invalid Enum Value");
#define HANDLE_UNARYOP(Val, Str)                                               \
  case UnaryOpKind::Val:                                                       \
    return Str;
#include "simplecc/AST/Enums.def"
  }
}

BasicTypeKind BasicTypeKindFromString(const std::string &s) {
#define HANDLE_BASICTYPE(Val, Str)                                             \
  if (s == Str)                                                                \
    return BasicTypeKind::Val;
#include "simplecc/AST/Enums.def"
  assert(false && "Invalid String Conversion");
}

const char *CStringFromBasicTypeKind(BasicTypeKind val) {
  switch (val) {
  default:
    assert(false && "Invalid Enum Value");
#define HANDLE_BASICTYPE(Val, Str)                                             \
  case BasicTypeKind::Val:                                                     \
    return Str;
#include "simplecc/AST/Enums.def"
  }
}

} // namespace simplecc