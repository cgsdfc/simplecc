#include "simplecc/Parse/AST.h"
#include <cassert>
#include <simplecc/Support/Casting.h>

using namespace simplecc;

template<typename AstT>
static inline void SetterImpl(AstT *&LHS, AstT *RHS, bool Optional = false) {
  assert((Optional || RHS) && "Only optional field can be null");
  if (LHS == RHS)
    return;
  DeleteAST::apply(LHS);
  LHS = RHS;
}

/// Implement the rvalue getter
template<typename AstT>
static inline UniquePtrToAST RvalueGetterImpl(AstT *&Ref) {
  AstT *tmp = nullptr;
  std::swap(tmp, Ref);
  return UniquePtrToAST(tmp);
}

Program::~Program() {
  DeleteAST::apply(decls);
}

FuncDef::~FuncDef() {
  DeleteAST::apply(args);
  DeleteAST::apply(decls);
  DeleteAST::apply(stmts);
}

ReadStmt::~ReadStmt() {
  DeleteAST::apply(names);
}

WriteStmt::~WriteStmt() {
  DeleteAST::apply(str);
  DeleteAST::apply(value);
}

void WriteStmt::setValue(Expr *Val) {
  SetterImpl(value, Val, /* Optional */ true);
}

AssignStmt::~AssignStmt() {
  DeleteAST::apply(target);
  DeleteAST::apply(value);
}

void AssignStmt::setValue(Expr *E) { SetterImpl(value, E); }

ForStmt::~ForStmt() {
  DeleteAST::apply(initial);
  DeleteAST::apply(condition);
  DeleteAST::apply(step);
  DeleteAST::apply(body);
}

void ForStmt::setCondition(Expr *E) { SetterImpl(condition, E); }

WhileStmt::~WhileStmt() {
  DeleteAST::apply(condition);
  DeleteAST::apply(body);
}

void WhileStmt::setCondition(Expr *E) { SetterImpl(condition, E); }

ReturnStmt::~ReturnStmt() { DeleteAST::apply(value); }

void ReturnStmt::setValue(Expr *E) {
  SetterImpl(value, E, /* Optional */ true);
}

IfStmt::~IfStmt() {
  DeleteAST::apply(test);
  DeleteAST::apply(body);
  DeleteAST::apply(orelse);
}

void IfStmt::setTest(Expr *E) {
  SetterImpl(test, E);
}

ExprStmt::~ExprStmt() {
  DeleteAST::apply(value);
}
UniquePtrToAST ExprStmt::getValue() &&{
  return RvalueGetterImpl(value);
}

BinOpExpr::~BinOpExpr() {
  DeleteAST::apply(left);
  DeleteAST::apply(right);
}

void BinOpExpr::setLeft(Expr *E) { SetterImpl(left, E); }

void BinOpExpr::setRight(Expr *E) { SetterImpl(right, E); }
UniquePtrToAST BinOpExpr::getLeft() &&{
  return RvalueGetterImpl(left);
}
UniquePtrToAST BinOpExpr::getRight() &&{
  return RvalueGetterImpl(right);
}

ParenExpr::~ParenExpr() { DeleteAST::apply(value); }

void ParenExpr::setValue(Expr *E) { SetterImpl(value, E); }
UniquePtrToAST ParenExpr::getValue() &&{
  return RvalueGetterImpl(value);
}

BoolOpExpr::~BoolOpExpr() { DeleteAST::apply(value); }

void BoolOpExpr::setValue(Expr *E) {
  SetterImpl(value, E);
  setHasCompareOp(
      IsInstance<BinOpExpr>(E) && isCompareOp(static_cast<BinOpExpr *>(E)->getOp()));
}

bool BoolOpExpr::isCompareOp(OperatorKind Op) {
  switch (Op) {
  default:return false;
#define HANDLE_COMPARE_OPERATOR(VAL, OP, FUNC) case OperatorKind::VAL: return true;
#include "simplecc/Parse/Enums.def"
  }
}

UniquePtrToAST BoolOpExpr::getValue() &&{
  return RvalueGetterImpl(value);
}

UnaryOpExpr::~UnaryOpExpr() { DeleteAST::apply(operand); }

void UnaryOpExpr::setOperand(Expr *E) { SetterImpl(operand, E); }

UniquePtrToAST UnaryOpExpr::getOperand() &&{
  return RvalueGetterImpl(operand);
}

CallExpr::~CallExpr() {
  DeleteAST::apply(args);
}

void CallExpr::setArgAt(unsigned I, Expr *Val) { SetterImpl(args[I], Val); }

SubscriptExpr::~SubscriptExpr() { DeleteAST::apply(index); }

void SubscriptExpr::setIndex(Expr *E) { SetterImpl(index, E); }

UniquePtrToAST SubscriptExpr::getIndex() &&{
  return RvalueGetterImpl(index);
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

int Expr::getConstantValue() const {
  assert(isConstant());
  switch (getKind()) {
  case CharExprKind:return static_cast<const CharExpr *>(this)->getC();
  case NumExprKind:return static_cast<const NumExpr *>(this)->getN();
  default:assert(false && "Unhandled Constant Expr");
  }
}


