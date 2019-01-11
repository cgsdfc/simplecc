#include "simplecc/Parse/Enums.h"
#include <cassert>

namespace simplecc {

std::ostream &operator<<(std::ostream &os, BinaryOpKind val) {
  switch (val) {
  default:
    assert(false && "Invalid Enum Value");
#define HANDLE_OPERATOR(VAL, STR, FUNC)                                        \
  case BinaryOpKind::VAL:                                                      \
    return os << #VAL;
#include "simplecc/Parse/Enums.def"
  }
}

std::ostream &operator<<(std::ostream &os, UnaryOpKind val) {
  switch (val) {
  default:
    assert(false && "Invalid Enum Value");
#define HANDLE_UNARYOP(VAL, STR)                                               \
  case UnaryOpKind::VAL:                                                       \
    return os << #VAL;
#include "simplecc/Parse/Enums.def"
  }
}

std::ostream &operator<<(std::ostream &os, ExprContextKind val) {
  switch (val) {
  default:
    assert(false && "Invalid Enum Value");
#define HANDLE_EXPRCONTEXT(VAL, STR)                                           \
  case ExprContextKind::VAL:                                                   \
    return os << #VAL;
#include "simplecc/Parse/Enums.def"
  }
}

std::ostream &operator<<(std::ostream &os, BasicTypeKind val) {
  switch (val) {
  default:
    assert(false && "Invalid Enum Value");
#define HANDLE_BASICTYPE(VAL, STR)                                             \
  case BasicTypeKind::VAL:                                                     \
    return os << #VAL;
#include "simplecc/Parse/Enums.def"
  }
}

BinaryOpKind OperatorKindFromString(const std::string &s) {
#define HANDLE_OPERATOR(Val, Str, FUNC)                                        \
  if (s == Str)                                                                \
    return BinaryOpKind::Val;
#include "simplecc/Parse/Enums.def"
  assert(false && "Invalid String Conversion");
}

const char *CStringFromOperatorKind(BinaryOpKind val) {
  switch (val) {
  default:
    assert(false && "Invalid Enum Value");
#define HANDLE_OPERATOR(Val, Str, FUNC)                                        \
  case BinaryOpKind::Val:                                                      \
    return Str;
#include "simplecc/Parse/Enums.def"
  }
}

UnaryOpKind UnaryopKindFromString(const std::string &s) {
#define HANDLE_UNARYOP(Val, Str)                                               \
  if (s == Str)                                                                \
    return UnaryOpKind::Val;
#include "simplecc/Parse/Enums.def"
  assert(false && "Invalid String Conversion");
}

const char *CStringFromUnaryopKind(UnaryOpKind val) {
  switch (val) {
  default:
    assert(false && "Invalid Enum Value");
#define HANDLE_UNARYOP(Val, Str)                                               \
  case UnaryOpKind::Val:                                                       \
    return Str;
#include "simplecc/Parse/Enums.def"
  }
}

BasicTypeKind BasicTypeKindFromString(const std::string &s) {
#define HANDLE_BASICTYPE(Val, Str)                                             \
  if (s == Str)                                                                \
    return BasicTypeKind::Val;
#include "simplecc/Parse/Enums.def"
  assert(false && "Invalid String Conversion");
}

const char *CStringFromBasicTypeKind(BasicTypeKind val) {
  switch (val) {
  default:
    assert(false && "Invalid Enum Value");
#define HANDLE_BASICTYPE(Val, Str)                                             \
  case BasicTypeKind::Val:                                                     \
    return Str;
#include "simplecc/Parse/Enums.def"
  }
}

} // namespace simplecc