#include "simplecc/Parse/AST.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

namespace simplecc {

template<typename AstT>
static inline void setterImpl(AstT *&LHS, AstT *RHS, bool Optional = false) {
  assert((Optional || RHS) && "Only optional field can be null");
  if (LHS == RHS)
    return;
  assert(LHS && "LHS cannot be null");
  LHS->deleteAST();
  LHS = RHS;
}

template<typename AstT>
static inline void deleteList(const std::vector<AstT *> &List) {
  std::for_each(List.begin(), List.end(), [](AstT *A) {
    assert(A && "AST in a List can't be null");
    A->deleteAST();
  });
}

static inline void deleteOptional(AST *A) {
  DeleteAST::apply(A);
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
  deleteList(decls);
}

FuncDef::~FuncDef() {
  deleteList(args);
  deleteList(decls);
  deleteList(stmts);
}

ReadStmt::~ReadStmt() {
  deleteList(names);
}

WriteStmt::~WriteStmt() {
  deleteOptional(str);
  deleteOptional(value);
}

void WriteStmt::setValue(Expr *Val) {
  setterImpl(value, Val, /* Optional */ true);
}

AssignStmt::~AssignStmt() {
  target->deleteAST();
  value->deleteAST();
}

void AssignStmt::setValue(Expr *E) { setterImpl(value, E); }

ForStmt::~ForStmt() {
  initial->deleteAST();
  condition->deleteAST();
  step->deleteAST();
  deleteList(body);
}

void ForStmt::setCondition(Expr *E) { setterImpl(condition, E); }

WhileStmt::~WhileStmt() {
  condition->deleteAST();
  deleteList(body);
}

void WhileStmt::setCondition(Expr *E) { setterImpl(condition, E); }

ReturnStmt::~ReturnStmt() { deleteOptional(value); }

void ReturnStmt::setValue(Expr *E) {
  setterImpl(value, E, /* Optional */ true);
}

IfStmt::~IfStmt() {
  test->deleteAST();
  deleteList(body);
  deleteList(orelse);
}

void IfStmt::setTest(Expr *E) {
  setterImpl(test, E);
}

ExprStmt::~ExprStmt() {
  value->deleteAST();
}

BinOpExpr::~BinOpExpr() {
  left->deleteAST();
  right->deleteAST();
}

void BinOpExpr::setLeft(Expr *E) { setterImpl(left, E); }

void BinOpExpr::setRight(Expr *E) { setterImpl(right, E); }

ParenExpr::~ParenExpr() { value->deleteAST(); }

void ParenExpr::setValue(Expr *E) { setterImpl(value, E); }

BoolOpExpr::~BoolOpExpr() { value->deleteAST(); }

void BoolOpExpr::setValue(Expr *E) { setterImpl(value, E); }

UnaryOpExpr::~UnaryOpExpr() { operand->deleteAST(); }

void UnaryOpExpr::setOperand(Expr *E) { setterImpl(operand, E); }

CallExpr::~CallExpr() {
  deleteList(args);
}

void CallExpr::setArgAt(unsigned I, Expr *Val) { setterImpl(args[I], Val); }

SubscriptExpr::~SubscriptExpr() { index->deleteAST(); }

void SubscriptExpr::setIndex(Expr *E) { setterImpl(index, E); }

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
  default:assert(false && "Unhandled AST Kind");
#define HANDLE_AST(CLASS)                                                      \
  case CLASS##Kind:                                                            \
    return #CLASS;
#include "simplecc/Parse/AST.def"
  }
}

void AST::deleteAST() {
  switch (getKind()) {
  default:assert(false && "Unhandled AST Kind");
#define HANDLE_AST(CLASS) \
  case CLASS##Kind: \
    delete static_cast<CLASS *>(this); \
    break;
#include "simplecc/Parse/AST.def"
  }
}

void AST::Format(std::ostream &os) const {
  os << "<" << getClassName() << " object at " << static_cast<const void *>(this)
     << ">";
}

} // namespace simplecc
