#include "simplecc/Parse/AST.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <simplecc/Parse/AST.h>

namespace simplecc {

// Format Optional Ast
std::ostream &operator<<(std::ostream &os, const AST *ast) {
  if (ast == nullptr)
    return os << "None";
  return os << *ast;
}

// Format Sequential Ast
template<typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
  os << "[";
  for (auto b = v.begin(), e = v.end(); b != e; ++b) {
    os << *b;
    if (b != e - 1) {
      os << ", ";
    }
  }
  os << "]";
  return os;
}

void Program::Format(std::ostream &os) const {
  os << "Program("
     << "decls=" << decls << ")";
}

void ConstDecl::Format(std::ostream &os) const {
  os << "ConstDecl("
     << "type=" << type << ", "
     << "value=" << value << ", "
     << "name=" << name << ", "
     << "loc=" << getLocation() << ")";
}

void VarDecl::Format(std::ostream &os) const {
  os << "VarDecl("
     << "type=" << type << ", "
     << "is_array=" << is_array << ", "
     << "size=" << size << ", "
     << "name=" << name << ", "
     << "loc=" << getLocation() << ")";
}

void FuncDef::Format(std::ostream &os) const {
  os << "FuncDef("
     << "return_type=" << return_type << ", "
     << "args=" << args << ", "
     << "decls=" << decls << ", "
     << "stmts=" << stmts << ", "
     << "name=" << name << ", "
     << "loc=" << getLocation() << ")";
}

void ArgDecl::Format(std::ostream &os) const {
  os << "ArgDecl("
     << "type=" << type << ", "
     << "name=" << name << ", "
     << "loc=" << getLocation() << ")";
}

void ReadStmt::Format(std::ostream &os) const {
  os << "ReadStmt("
     << "names=" << names << ", "
     << "loc=" << getLocation() << ")";
}

void WriteStmt::Format(std::ostream &os) const {
  os << "WriteStmt("
     << "str=" << str << ", "
     << "value=" << value << ", "
     << "loc=" << getLocation() << ")";
}

void AssignStmt::Format(std::ostream &os) const {
  os << "AssignStmt("
     << "target=" << target << ", "
     << "value=" << value << ", "
     << "loc=" << getLocation() << ")";
}

void ForStmt::Format(std::ostream &os) const {
  os << "ForStmt("
     << "initial=" << initial << ", "
     << "condition=" << condition << ", "
     << "step=" << step << ", "
     << "body=" << body << ", "
     << "loc=" << getLocation() << ")";
}

void WhileStmt::Format(std::ostream &os) const {
  os << "WhileStmt("
     << "condition=" << condition << ", "
     << "body=" << body << ", "
     << "loc=" << getLocation() << ")";
}

void ReturnStmt::Format(std::ostream &os) const {
  os << "ReturnStmt("
     << "value=" << value << ", "
     << "loc=" << getLocation() << ")";
}

void IfStmt::Format(std::ostream &os) const {
  os << "IfStmt("
     << "test=" << test << ", "
     << "body=" << body << ", "
     << "orelse=" << orelse << ", "
     << "loc=" << getLocation() << ")";
}

void ExprStmt::Format(std::ostream &os) const {
  os << "ExprStmt("
     << "value=" << value << ", "
     << "loc=" << getLocation() << ")";
}

void BinOpExpr::Format(std::ostream &os) const {
  os << "BinOpExpr("
     << "left=" << left << ", "
     << "op=" << op << ", "
     << "right=" << right << ", "
     << "loc=" << getLocation() << ")";
}

void ParenExpr::Format(std::ostream &os) const {
  os << "ParenExpr("
     << "value=" << value << ", "
     << "loc=" << getLocation() << ")";
}

void BoolOpExpr::Format(std::ostream &os) const {
  os << "BoolOpExpr("
     << "value=" << value << ", "
     << "has_cmpop=" << has_cmpop << ", "
     << "loc=" << getLocation() << ")";
}

void UnaryOpExpr::Format(std::ostream &os) const {
  os << "UnaryOpExpr("
     << "op=" << op << ", "
     << "operand=" << operand << ", "
     << "loc=" << getLocation() << ")";
}

void CallExpr::Format(std::ostream &os) const {
  os << "CallExpr("
     << "func=" << func << ", "
     << "args=" << args << ", "
     << "loc=" << getLocation() << ")";
}

void NumExpr::Format(std::ostream &os) const {
  os << "NumExpr("
     << "n=" << n << ", "
     << "loc=" << getLocation() << ")";
}

void StrExpr::Format(std::ostream &os) const {
  os << "StrExpr("
     << "s=" << s << ", "
     << "loc=" << getLocation() << ")";
}

void CharExpr::Format(std::ostream &os) const {
  os << "CharExpr("
     << "c=" << c << ", "
     << "loc=" << getLocation() << ")";
}

void SubscriptExpr::Format(std::ostream &os) const {
  os << "SubscriptExpr("
     << "name=" << name << ", "
     << "index=" << index << ", "
     << "ctx=" << ctx << ", "
     << "loc=" << getLocation() << ")";
}

void NameExpr::Format(std::ostream &os) const {
  os << "NameExpr("
     << "id=" << id << ", "
     << "ctx=" << ctx << ", "
     << "loc=" << getLocation() << ")";
}

std::ostream &operator<<(std::ostream &os, OperatorKind val) {
  switch (val) {
  case OperatorKind::Add:return os << "OperatorKind::Add";
  case OperatorKind::Sub:return os << "OperatorKind::Sub";
  case OperatorKind::Mult:return os << "OperatorKind::Mult";
  case OperatorKind::Div:return os << "OperatorKind::Div";
  case OperatorKind::Eq:return os << "OperatorKind::Eq";
  case OperatorKind::NotEq:return os << "OperatorKind::NotEq";
  case OperatorKind::Lt:return os << "OperatorKind::Lt";
  case OperatorKind::LtE:return os << "OperatorKind::LtE";
  case OperatorKind::Gt:return os << "OperatorKind::Gt";
  case OperatorKind::GtE:return os << "OperatorKind::GtE";
  }
}

std::ostream &operator<<(std::ostream &os, UnaryopKind val) {
  switch (val) {
  case UnaryopKind::UAdd:return os << "UnaryopKind::UAdd";
  case UnaryopKind::USub:return os << "UnaryopKind::USub";
  }
}

std::ostream &operator<<(std::ostream &os, ExprContextKind val) {
  switch (val) {
  case ExprContextKind::Load:return os << "ExprContextKind::Load";
  case ExprContextKind::Store:return os << "ExprContextKind::Store";
  }
}

std::ostream &operator<<(std::ostream &os, BasicTypeKind val) {
  switch (val) {
  case BasicTypeKind::Int:return os << "BasicTypeKind::Int";
  case BasicTypeKind::Character:return os << "BasicTypeKind::Character";
  case BasicTypeKind::Void:return os << "BasicTypeKind::Void";
  }
}

Program::~Program() {
  for (auto v : decls)
    delete v;
}

ConstDecl::~ConstDecl() { delete value; }

VarDecl::~VarDecl() = default;

FuncDef::~FuncDef() {
  for (auto v : args)
    delete v;
  for (auto v : decls)
    delete v;
  for (auto v : stmts)
    delete v;
}

ArgDecl::~ArgDecl() = default;

ReadStmt::~ReadStmt() {
  for (auto v : names)
    delete v;
}

WriteStmt::~WriteStmt() {
  delete str;
  delete value;
}

AssignStmt::~AssignStmt() {
  delete target;
  delete value;
}

ForStmt::~ForStmt() {
  delete initial;
  delete condition;
  delete step;
  for (auto v : body)
    delete v;
}

WhileStmt::~WhileStmt() {
  delete condition;
  for (auto v : body)
    delete v;
}

ReturnStmt::~ReturnStmt() { delete value; }

IfStmt::~IfStmt() {
  delete test;
  for (auto v : body)
    delete v;
  for (auto v : orelse)
    delete v;
}

ExprStmt::~ExprStmt() { delete value; }

BinOpExpr::~BinOpExpr() {
  delete left;
  delete right;
}

ParenExpr::~ParenExpr() { delete value; }

BoolOpExpr::~BoolOpExpr() { delete value; }

UnaryOpExpr::~UnaryOpExpr() { delete operand; }

CallExpr::~CallExpr() {
  for (auto v : args)
    delete v;
}

NumExpr::~NumExpr() = default;
StrExpr::~StrExpr() = default;
CharExpr::~CharExpr() = default;

SubscriptExpr::~SubscriptExpr() { delete index; }

NameExpr::~NameExpr() = default;

OperatorKind OperatorKindFromString(const String &s) {
  if (s == "+")
    return OperatorKind::Add;
  if (s == "-")
    return OperatorKind::Sub;
  if (s == "*")
    return OperatorKind::Mult;
  if (s == "/")
    return OperatorKind::Div;
  if (s == "==")
    return OperatorKind::Eq;
  if (s == "!=")
    return OperatorKind::NotEq;
  if (s == "<")
    return OperatorKind::Lt;
  if (s == "<=")
    return OperatorKind::LtE;
  if (s == ">")
    return OperatorKind::Gt;
  if (s == ">=")
    return OperatorKind::GtE;
  assert(false && "not a member of OperatorKind");
}

const char *CStringFromOperatorKind(OperatorKind val) {
  switch (val) {
  case OperatorKind::Add:return "+";
  case OperatorKind::Sub:return "-";
  case OperatorKind::Mult:return "*";
  case OperatorKind::Div:return "/";
  case OperatorKind::Eq:return "==";
  case OperatorKind::NotEq:return "!=";
  case OperatorKind::Lt:return "<";
  case OperatorKind::LtE:return "<=";
  case OperatorKind::Gt:return ">";
  case OperatorKind::GtE:return ">=";
  }
}

UnaryopKind UnaryopKindFromString(const String &s) {
  if (s == "+")
    return UnaryopKind::UAdd;
  if (s == "-")
    return UnaryopKind::USub;
  assert(false && "not a member of UnaryopKind");
}

const char *CStringFromUnaryopKind(UnaryopKind val) {
  switch (val) {
  case UnaryopKind::UAdd:return "+";
  case UnaryopKind::USub:return "-";
  }
}

BasicTypeKind BasicTypeKindFromString(const String &s) {
  if (s == "int")
    return BasicTypeKind::Int;
  if (s == "char")
    return BasicTypeKind::Character;
  if (s == "void")
    return BasicTypeKind::Void;
  assert(false && "not a member of BasicTypeKind");
}

const char *CStringFromBasicTypeKind(BasicTypeKind val) {
  switch (val) {
  case BasicTypeKind::Int:return "int";
  case BasicTypeKind::Character:return "char";
  case BasicTypeKind::Void:return "void";
  }
}

const char *AST::getClassName(unsigned Kind) {
  switch (Kind) {
  default: assert(false && "Unhandled AST Kind");
#define HANDLE_AST(CLASS) case CLASS##Kind: return #CLASS;
#include "simplecc/Parse/AST.def"
  }
}

} // namespace simplecc
