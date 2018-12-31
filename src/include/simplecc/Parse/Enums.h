#ifndef SIMPLECC_PARSE_ENUMS_H
#define SIMPLECC_PARSE_ENUMS_H
#include <iostream>
#include <string>

namespace simplecc {
enum class OperatorKind {
#define HANDLE_OPERATOR(Val, Str, FUNC) Val,
#include "simplecc/Parse/Enums.def"
};

enum class UnaryopKind {
#define HANDLE_UNARYOP(Val, Str) Val,
#include "simplecc/Parse/Enums.def"
};

enum class ExprContextKind {
#define HANDLE_EXPRCONTEXT(Val, Str) Val,
#include "simplecc/Parse/Enums.def"
};

enum class BasicTypeKind {
#define HANDLE_BASICTYPE(Val, Str) Val,
#include "simplecc/Parse/Enums.def"
};


std::ostream &operator<<(std::ostream &os, OperatorKind val);
std::ostream &operator<<(std::ostream &os, UnaryopKind val);
std::ostream &operator<<(std::ostream &os, ExprContextKind val);
std::ostream &operator<<(std::ostream &os, BasicTypeKind val);

OperatorKind OperatorKindFromString(const std::string &s);
const char *CStringFromOperatorKind(OperatorKind val);

UnaryopKind UnaryopKindFromString(const std::string &s);
const char *CStringFromUnaryopKind(UnaryopKind val);

BasicTypeKind BasicTypeKindFromString(const std::string &s);
const char *CStringFromBasicTypeKind(BasicTypeKind val);
} // namespace simplecc
#endif //SIMPLECC_PARSE_ENUMS_H
