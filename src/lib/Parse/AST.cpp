#include "simplecc/Parse/AST.h"
#include <cassert>
#include <algorithm>

using namespace simplecc;

template<typename AstT>
static inline void setterImpl(AstT *&LHS, AstT *RHS, bool Optional = false) {
  assert((Optional || RHS) && "Only optional field can be null");
  if (LHS == RHS)
    return;
  DeleteAST::apply(LHS);
  LHS = RHS;
}

/// Implement the rvalue getter
template<typename AstT>
static inline UniquePtrToAST rvalueGetterImpl(AstT *&Ref) {
  AstT *tmp = nullptr;
  std::swap(tmp, Ref);
  return UniquePtrToAST(tmp);
}

template<typename AstT>
static inline void deleteList(const std::vector<AstT *> &List) {
  std::for_each(List.begin(), List.end(), [](AstT *A) {
    DeleteAST::apply(A);
  });
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
  DeleteAST::apply(str);
  DeleteAST::apply(value);
}

void WriteStmt::setValue(Expr *Val) {
  setterImpl(value, Val, /* Optional */ true);
}

AssignStmt::~AssignStmt() {
  DeleteAST::apply(target);
  DeleteAST::apply(value);
}

void AssignStmt::setValue(Expr *E) { setterImpl(value, E); }

ForStmt::~ForStmt() {
  DeleteAST::apply(initial);
  DeleteAST::apply(condition);
  DeleteAST::apply(step);
  deleteList(body);
}

void ForStmt::setCondition(Expr *E) { setterImpl(condition, E); }

WhileStmt::~WhileStmt() {
  DeleteAST::apply(condition);
  deleteList(body);
}

void WhileStmt::setCondition(Expr *E) { setterImpl(condition, E); }

ReturnStmt::~ReturnStmt() { DeleteAST::apply(value); }

void ReturnStmt::setValue(Expr *E) {
  setterImpl(value, E, /* Optional */ true);
}

IfStmt::~IfStmt() {
  DeleteAST::apply(test);
  deleteList(body);
  deleteList(orelse);
}

void IfStmt::setTest(Expr *E) {
  setterImpl(test, E);
}

ExprStmt::~ExprStmt() {
  DeleteAST::apply(value);
}

BinOpExpr::~BinOpExpr() {
  DeleteAST::apply(left);
  DeleteAST::apply(right);
}

void BinOpExpr::setLeft(Expr *E) { setterImpl(left, E); }

void BinOpExpr::setRight(Expr *E) { setterImpl(right, E); }

ParenExpr::~ParenExpr() { DeleteAST::apply(value); }

void ParenExpr::setValue(Expr *E) { setterImpl(value, E); }

BoolOpExpr::~BoolOpExpr() { DeleteAST::apply(value); }

void BoolOpExpr::setValue(Expr *E) { setterImpl(value, E); }

UnaryOpExpr::~UnaryOpExpr() { DeleteAST::apply(operand); }

void UnaryOpExpr::setOperand(Expr *E) { setterImpl(operand, E); }

CallExpr::~CallExpr() {
  deleteList(args);
}

void CallExpr::setArgAt(unsigned I, Expr *Val) { setterImpl(args[I], Val); }

SubscriptExpr::~SubscriptExpr() { DeleteAST::apply(index); }

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
  PrettyPrintAST(*this, os);
}

bool Decl::InstanceCheck(const AST *A) {
  switch (A->getKind()) {
  default:return false;
#define HANDLE_DECL(CLASS) case AST::CLASS##Kind:
#include "simplecc/Parse/AST.def"
    return true;
  }
}

bool Stmt::InstanceCheck(const AST *A) {
  switch (A->getKind()) {
  default:return false;
#define HANDLE_STMT(CLASS) case AST::CLASS##Kind:
#include "simplecc/Parse/AST.def"
    return true;
  }
}

bool Expr::InstanceCheck(const AST *A) {
  switch (A->getKind()) {
  default:return false;
#define HANDLE_EXPR(CLASS) case AST::CLASS##Kind:
#include "simplecc/Parse/AST.def"
    return true;
  }
}