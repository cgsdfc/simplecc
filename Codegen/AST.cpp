
#include "AST.h"

namespace simplecompiler {
template <class T>
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

template <class T>
std::ostream &operator<<(std::ostream &os, const std::optional<T> &v) {
  if (v.has_value())
    return os << v.value();
  else
    return os << "None";
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
     << "loc=" << loc << ")";
}

void VarDecl::Format(std::ostream &os) const {
  os << "VarDecl("
     << "type=" << type << ", "
     << "is_array=" << is_array << ", "
     << "size=" << size << ", "
     << "name=" << name << ", "
     << "loc=" << loc << ")";
}

void FuncDef::Format(std::ostream &os) const {
  os << "FuncDef("
     << "return_type=" << return_type << ", "
     << "args=" << args << ", "
     << "decls=" << decls << ", "
     << "stmts=" << stmts << ", "
     << "name=" << name << ", "
     << "loc=" << loc << ")";
}

void Arg::Format(std::ostream &os) const {
  os << "Arg("
     << "type=" << type << ", "
     << "name=" << name << ", "
     << "loc=" << loc << ")";
}

void Read::Format(std::ostream &os) const {
  os << "Read("
     << "names=" << names << ", "
     << "loc=" << loc << ")";
}

void Write::Format(std::ostream &os) const {
  os << "Write("
     << "str=" << str << ", "
     << "value=" << value << ", "
     << "loc=" << loc << ")";
}

void Assign::Format(std::ostream &os) const {
  os << "Assign("
     << "target=" << target << ", "
     << "value=" << value << ", "
     << "loc=" << loc << ")";
}

void For::Format(std::ostream &os) const {
  os << "For("
     << "initial=" << initial << ", "
     << "condition=" << condition << ", "
     << "step=" << step << ", "
     << "body=" << body << ", "
     << "loc=" << loc << ")";
}

void While::Format(std::ostream &os) const {
  os << "While("
     << "condition=" << condition << ", "
     << "body=" << body << ", "
     << "loc=" << loc << ")";
}

void Return::Format(std::ostream &os) const {
  os << "Return("
     << "value=" << value << ", "
     << "loc=" << loc << ")";
}

void If::Format(std::ostream &os) const {
  os << "If("
     << "test=" << test << ", "
     << "body=" << body << ", "
     << "orelse=" << orelse << ", "
     << "loc=" << loc << ")";
}

void ExprStmt::Format(std::ostream &os) const {
  os << "ExprStmt("
     << "value=" << value << ", "
     << "loc=" << loc << ")";
}

void BinOp::Format(std::ostream &os) const {
  os << "BinOp("
     << "left=" << left << ", "
     << "op=" << op << ", "
     << "right=" << right << ", "
     << "loc=" << loc << ")";
}

void ParenExpr::Format(std::ostream &os) const {
  os << "ParenExpr("
     << "value=" << value << ", "
     << "loc=" << loc << ")";
}

void BoolOp::Format(std::ostream &os) const {
  os << "BoolOp("
     << "value=" << value << ", "
     << "has_cmpop=" << has_cmpop << ", "
     << "loc=" << loc << ")";
}

void UnaryOp::Format(std::ostream &os) const {
  os << "UnaryOp("
     << "op=" << op << ", "
     << "operand=" << operand << ", "
     << "loc=" << loc << ")";
}

void Call::Format(std::ostream &os) const {
  os << "Call("
     << "func=" << func << ", "
     << "args=" << args << ", "
     << "loc=" << loc << ")";
}

void Num::Format(std::ostream &os) const {
  os << "Num("
     << "n=" << n << ", "
     << "loc=" << loc << ")";
}

void Str::Format(std::ostream &os) const {
  os << "Str("
     << "s=" << s << ", "
     << "loc=" << loc << ")";
}

void Char::Format(std::ostream &os) const {
  os << "Char("
     << "c=" << c << ", "
     << "loc=" << loc << ")";
}

void Subscript::Format(std::ostream &os) const {
  os << "Subscript("
     << "name=" << name << ", "
     << "index=" << index << ", "
     << "ctx=" << ctx << ", "
     << "loc=" << loc << ")";
}

void Name::Format(std::ostream &os) const {
  os << "Name("
     << "id=" << id << ", "
     << "ctx=" << ctx << ", "
     << "loc=" << loc << ")";
}

std::ostream &operator<<(std::ostream &os, OperatorKind val) {
  switch (val) {

  case OperatorKind::Add:
    return os << "OperatorKind::Add";

  case OperatorKind::Sub:
    return os << "OperatorKind::Sub";

  case OperatorKind::Mult:
    return os << "OperatorKind::Mult";

  case OperatorKind::Div:
    return os << "OperatorKind::Div";

  case OperatorKind::Eq:
    return os << "OperatorKind::Eq";

  case OperatorKind::NotEq:
    return os << "OperatorKind::NotEq";

  case OperatorKind::Lt:
    return os << "OperatorKind::Lt";

  case OperatorKind::LtE:
    return os << "OperatorKind::LtE";

  case OperatorKind::Gt:
    return os << "OperatorKind::Gt";

  case OperatorKind::GtE:
    return os << "OperatorKind::GtE";
  }
}

std::ostream &operator<<(std::ostream &os, UnaryopKind val) {
  switch (val) {

  case UnaryopKind::UAdd:
    return os << "UnaryopKind::UAdd";

  case UnaryopKind::USub:
    return os << "UnaryopKind::USub";
  }
}

std::ostream &operator<<(std::ostream &os, ExprContextKind val) {
  switch (val) {

  case ExprContextKind::Load:
    return os << "ExprContextKind::Load";

  case ExprContextKind::Store:
    return os << "ExprContextKind::Store";
  }
}

std::ostream &operator<<(std::ostream &os, BasicTypeKind val) {
  switch (val) {

  case BasicTypeKind::Int:
    return os << "BasicTypeKind::Int";

  case BasicTypeKind::Character:
    return os << "BasicTypeKind::Character";

  case BasicTypeKind::Void:
    return os << "BasicTypeKind::Void";
  }
}

Program::~Program() {
  for (auto v : decls)
    delete v;
}

ConstDecl::~ConstDecl() { delete value; }

VarDecl::~VarDecl() {}

FuncDef::~FuncDef() {
  for (auto v : args)
    delete v;
  for (auto v : decls)
    delete v;
  for (auto v : stmts)
    delete v;
}

Arg::~Arg() {}

Read::~Read() {
  for (auto v : names)
    delete v;
}

Write::~Write() {
  if (str)
    delete str;
  if (value)
    delete value;
}

Assign::~Assign() {
  delete target;
  delete value;
}

For::~For() {
  delete initial;
  delete condition;
  delete step;
  for (auto v : body)
    delete v;
}

While::~While() {
  delete condition;
  for (auto v : body)
    delete v;
}

Return::~Return() {
  if (value)
    delete value;
}

If::~If() {
  delete test;
  for (auto v : body)
    delete v;
  for (auto v : orelse)
    delete v;
}

ExprStmt::~ExprStmt() { delete value; }

BinOp::~BinOp() {
  delete left;
  delete right;
}

ParenExpr::~ParenExpr() { delete value; }

BoolOp::~BoolOp() { delete value; }

UnaryOp::~UnaryOp() { delete operand; }

Call::~Call() {
  for (auto v : args)
    delete v;
}

Num::~Num() {}

Str::~Str() {}

Char::~Char() {}

Subscript::~Subscript() { delete index; }

Name::~Name() {}

OperatorKind String2OperatorKind(const String &s) {

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

  case OperatorKind::Add:
    return "+";

  case OperatorKind::Sub:
    return "-";

  case OperatorKind::Mult:
    return "*";

  case OperatorKind::Div:
    return "/";

  case OperatorKind::Eq:
    return "==";

  case OperatorKind::NotEq:
    return "!=";

  case OperatorKind::Lt:
    return "<";

  case OperatorKind::LtE:
    return "<=";

  case OperatorKind::Gt:
    return ">";

  case OperatorKind::GtE:
    return ">=";
  }
}

UnaryopKind String2UnaryopKind(const String &s) {

  if (s == "+")
    return UnaryopKind::UAdd;

  if (s == "-")
    return UnaryopKind::USub;

  assert(false && "not a member of UnaryopKind");
}

const char *CStringFromUnaryopKind(UnaryopKind val) {
  switch (val) {

  case UnaryopKind::UAdd:
    return "+";

  case UnaryopKind::USub:
    return "-";
  }
}

BasicTypeKind String2BasicTypeKind(const String &s) {

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

  case BasicTypeKind::Int:
    return "int";

  case BasicTypeKind::Character:
    return "char";

  case BasicTypeKind::Void:
    return "void";
  }
}

} // namespace simplecompiler
