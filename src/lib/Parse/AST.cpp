#include "simplecc/Parse/AST.h"
#include <simplecc/Support/Casting.h>
#include <simplecc/Parse/AST.h>

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

ProgramAST::~ProgramAST() { DeleteAST::apply(Decls); }

FuncDef::~FuncDef() {
  DeleteAST::apply(Args);
  DeleteAST::apply(Decls);
  DeleteAST::apply(Stmts);
}

ArgDecl *FuncDef::getArgAt(unsigned Pos) const {
  assert(Pos < Args.size() && "Pos out of range");
  auto Arg = Args[Pos];
  assert(IsInstance<ArgDecl>(Arg) && "Not a ArgDecl!");
  return static_cast<ArgDecl *>(Arg);
}

ReadStmt::~ReadStmt() { DeleteAST::apply(names); }

WriteStmt::~WriteStmt() {
  DeleteAST::apply(str);
  DeleteAST::apply(value);
}

void WriteStmt::setValue(ExprAST *Val) {
  SetterImpl(value, Val, /* Optional */ true);
}

AssignStmt::~AssignStmt() {
  DeleteAST::apply(target);
  DeleteAST::apply(value);
}

void AssignStmt::setValue(ExprAST *E) { SetterImpl(value, E); }
void AssignStmt::setTarget(ExprAST *E) { SetterImpl(target, E); }

ForStmt::~ForStmt() {
  DeleteAST::apply(initial);
  DeleteAST::apply(condition);
  DeleteAST::apply(step);
  DeleteAST::apply(body);
}

void ForStmt::setCondition(ExprAST *E) { SetterImpl(condition, E); }
UniquePtrToAST ForStmt::getInitial() &&{ return RvalueGetterImpl(initial); }
UniquePtrToAST ForStmt::getCondition() &&{
  return RvalueGetterImpl(condition);
}
UniquePtrToAST ForStmt::getStep() &&{ return RvalueGetterImpl(step); }

WhileStmt::~WhileStmt() {
  DeleteAST::apply(condition);
  DeleteAST::apply(body);
}

void WhileStmt::setCondition(ExprAST *E) { SetterImpl(condition, E); }

ReturnStmt::~ReturnStmt() { DeleteAST::apply(value); }

void ReturnStmt::setValue(ExprAST *E) {
  SetterImpl(value, E, /* Optional */ true);
}

IfStmt::~IfStmt() {
  DeleteAST::apply(test);
  DeleteAST::apply(body);
  DeleteAST::apply(orelse);
}

void IfStmt::setTest(ExprAST *E) { SetterImpl(test, E); }

ExprStmt::~ExprStmt() { DeleteAST::apply(value); }
UniquePtrToAST ExprStmt::getValue() &&{ return RvalueGetterImpl(value); }
void ExprStmt::setValue(ExprAST *E) {
  SetterImpl(value, E);
}

BinOpExpr::~BinOpExpr() {
  DeleteAST::apply(left);
  DeleteAST::apply(right);
}

void BinOpExpr::setLeft(ExprAST *E) { SetterImpl(left, E); }

void BinOpExpr::setRight(ExprAST *E) { SetterImpl(right, E); }
UniquePtrToAST BinOpExpr::getLeft() &&{ return RvalueGetterImpl(left); }
UniquePtrToAST BinOpExpr::getRight() &&{ return RvalueGetterImpl(right); }

ParenExpr::~ParenExpr() { DeleteAST::apply(value); }

void ParenExpr::setValue(ExprAST *E) { SetterImpl(value, E); }
UniquePtrToAST ParenExpr::getValue() &&{ return RvalueGetterImpl(value); }

BoolOpExpr::~BoolOpExpr() { DeleteAST::apply(value); }

void BoolOpExpr::setValue(ExprAST *E) {
  SetterImpl(value, E);
  setHasCompareOp(IsInstance<BinOpExpr>(E) &&
      isCompareOp(static_cast<BinOpExpr *>(E)->getOp()));
}

bool BoolOpExpr::isCompareOp(OperatorKind Op) {
  switch (Op) {
  default:return false;
#define HANDLE_COMPARE_OPERATOR(VAL, OP, FUNC)                                 \
  case OperatorKind::VAL:                                                      \
    return true;
#include "simplecc/Parse/Enums.def"
  }
}

UniquePtrToAST BoolOpExpr::getValue() &&{ return RvalueGetterImpl(value); }

UnaryOpExpr::~UnaryOpExpr() { DeleteAST::apply(operand); }

void UnaryOpExpr::setOperand(ExprAST *E) { SetterImpl(operand, E); }

UniquePtrToAST UnaryOpExpr::getOperand() &&{
  return RvalueGetterImpl(operand);
}

CallExpr::~CallExpr() { DeleteAST::apply(Args); }

void CallExpr::setArgAt(unsigned I, ExprAST *Val) { SetterImpl(Args[I], Val); }

SubscriptExpr::~SubscriptExpr() { DeleteAST::apply(index); }

void SubscriptExpr::setIndex(ExprAST *E) { SetterImpl(index, E); }

UniquePtrToAST SubscriptExpr::getIndex() &&{ return RvalueGetterImpl(index); }

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
#define HANDLE_AST(CLASS)                                                      \
  case CLASS##Kind:                                                            \
    delete static_cast<CLASS *>(this);                                         \
    break;
#include "simplecc/Parse/AST.def"
  }
}

void AST::Format(std::ostream &os) const { PrettyPrintAST(*this, os); }
void AST::dump() const { return Format(std::cerr); }

bool DeclAST::InstanceCheck(const AST *A) {
  switch (A->getKind()) {
  default:return false;
#define HANDLE_DECL(CLASS)                                                     \
  case AST::CLASS##Kind:                                                       \
    return true;
#include "simplecc/Parse/AST.def"
  }
}

bool StmtAST::InstanceCheck(const AST *A) {
  switch (A->getKind()) {
  default:return false;
#define HANDLE_STMT(CLASS)                                                     \
  case AST::CLASS##Kind:                                                       \
    return true;
#include "simplecc/Parse/AST.def"
  }
}

bool ExprAST::InstanceCheck(const AST *A) {
  switch (A->getKind()) {
  default:return false;
#define HANDLE_EXPR(CLASS)                                                     \
  case AST::CLASS##Kind:                                                       \
    return true;
#include "simplecc/Parse/AST.def"
  }
}

int ExprAST::getConstantValue() const {
  assert(isConstant() && "getConstantValue() on non-constant!");
  switch (getKind()) {
#define HANDLE_EXPR(CLASS)                                                     \
  case ExprAST::CLASS##Kind:                                                   \
    return static_cast<const CLASS *>(this)->getConstantValueImpl();
#include "simplecc/Parse/AST.def"
  default:assert(false && "Unhandled Enum Value");
  }
}

bool ExprAST::isConstant() const {
  switch (getKind()) {
#define HANDLE_EXPR(CLASS)                                                     \
  case ExprAST::CLASS##Kind:                                                   \
    return static_cast<const CLASS *>(this)->isConstantImpl();
#include "simplecc/Parse/AST.def"
  default:assert(false && "Unhandled Enum Value");
  }
}
