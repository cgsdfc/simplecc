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