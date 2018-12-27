#include "simplecc/Parse/AST.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <simplecc/Parse/AST.h>

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

bool Decl::InstanceCheck(AST *A) {
  switch (A->getKind()) {
  default:return false;
#define HANDLE_DECL(CLASS) case AST::CLASS##Kind:
#include "simplecc/Parse/AST.def"
    return true;
  }
}

bool Stmt::InstanceCheck(AST *A) {
  switch (A->getKind()) {
  default:return false;
#define HANDLE_STMT(CLASS) case AST::CLASS##Kind:
#include "simplecc/Parse/AST.def"
    return true;
  }
}

bool Expr::InstanceCheck(AST *A) {
  switch (A->getKind()) {
  default:return false;
#define HANDLE_EXPR(CLASS) case AST::CLASS##Kind:
#include "simplecc/Parse/AST.def"
    return true;
  }
}

} // namespace simplecc
