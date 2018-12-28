#include "simplecc/Parse/AST.h"
#include <cassert>
#include <algorithm>

using namespace simplecc;

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

static inline void deleteRequired(AST *A) {
  assert(A && "Required field can't be null");
  A->deleteAST();
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
  deleteRequired(target);
  deleteRequired(value);
}

void AssignStmt::setValue(Expr *E) { setterImpl(value, E); }

ForStmt::~ForStmt() {
  deleteRequired(initial);
  deleteRequired(condition);
  deleteRequired(step);
  deleteList(body);
}

void ForStmt::setCondition(Expr *E) { setterImpl(condition, E); }

WhileStmt::~WhileStmt() {
  deleteRequired(condition);
  deleteList(body);
}

void WhileStmt::setCondition(Expr *E) { setterImpl(condition, E); }

ReturnStmt::~ReturnStmt() { deleteOptional(value); }

void ReturnStmt::setValue(Expr *E) {
  setterImpl(value, E, /* Optional */ true);
}

IfStmt::~IfStmt() {
  deleteRequired(test);
  deleteList(body);
  deleteList(orelse);
}

void IfStmt::setTest(Expr *E) {
  setterImpl(test, E);
}

ExprStmt::~ExprStmt() {
  deleteRequired(value);
}

BinOpExpr::~BinOpExpr() {
 deleteRequired(left);
 deleteRequired(right);
}

void BinOpExpr::setLeft(Expr *E) { setterImpl(left, E); }

void BinOpExpr::setRight(Expr *E) { setterImpl(right, E); }

ParenExpr::~ParenExpr() { deleteRequired(value); }

void ParenExpr::setValue(Expr *E) { setterImpl(value, E); }

BoolOpExpr::~BoolOpExpr() { deleteRequired(value); }

void BoolOpExpr::setValue(Expr *E) { setterImpl(value, E); }

UnaryOpExpr::~UnaryOpExpr() { deleteRequired(operand); }

void UnaryOpExpr::setOperand(Expr *E) { setterImpl(operand, E); }

CallExpr::~CallExpr() {
  deleteList(args);
}

void CallExpr::setArgAt(unsigned I, Expr *Val) { setterImpl(args[I], Val); }

SubscriptExpr::~SubscriptExpr() { deleteRequired(index); }

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