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

ReadStmt::~ReadStmt() { DeleteAST::apply(Names); }

WriteStmt::~WriteStmt() {
  DeleteAST::apply(Str);
  DeleteAST::apply(Value);
}

void WriteStmt::setValue(ExprAST *Val) {
  SetterImpl(Value, Val, /* Optional */ true);
}

AssignStmt::~AssignStmt() {
  DeleteAST::apply(Target);
  DeleteAST::apply(Value);
}

void AssignStmt::setValue(ExprAST *E) { SetterImpl(Value, E); }
void AssignStmt::setTarget(ExprAST *E) { SetterImpl(Target, E); }

ForStmt::~ForStmt() {
  DeleteAST::apply(Initial);
  DeleteAST::apply(Cond);
  DeleteAST::apply(Step);
  DeleteAST::apply(Body);
}

void ForStmt::setCondition(ExprAST *E) { SetterImpl(Cond, E); }
UniquePtrToAST ForStmt::getInitial() &&{ return RvalueGetterImpl(Initial); }
UniquePtrToAST ForStmt::getCondition() &&{
  return RvalueGetterImpl(Cond);
}
UniquePtrToAST ForStmt::getStep() &&{ return RvalueGetterImpl(Step); }

WhileStmt::~WhileStmt() {
  DeleteAST::apply(Cond);
  DeleteAST::apply(Body);
}

void WhileStmt::setCondition(ExprAST *E) { SetterImpl(Cond, E); }

ReturnStmt::~ReturnStmt() { DeleteAST::apply(Value); }

void ReturnStmt::setValue(ExprAST *E) {
  SetterImpl(Value, E, /* Optional */ true);
}

IfStmt::~IfStmt() {
  DeleteAST::apply(Cond);
  DeleteAST::apply(Then);
  DeleteAST::apply(Else);
}

void IfStmt::setCondition(ExprAST *E) { SetterImpl(Cond, E); }

ExprStmt::~ExprStmt() { DeleteAST::apply(Value); }
UniquePtrToAST ExprStmt::getValue() &&{ return RvalueGetterImpl(Value); }
void ExprStmt::setValue(ExprAST *E) {
  SetterImpl(Value, E);
}

BinOpExpr::~BinOpExpr() {
  DeleteAST::apply(Left);
  DeleteAST::apply(Right);
}

void BinOpExpr::setLeft(ExprAST *E) { SetterImpl(Left, E); }

void BinOpExpr::setRight(ExprAST *E) { SetterImpl(Right, E); }
UniquePtrToAST BinOpExpr::getLeft() &&{ return RvalueGetterImpl(Left); }
UniquePtrToAST BinOpExpr::getRight() &&{ return RvalueGetterImpl(Right); }

ParenExpr::~ParenExpr() { DeleteAST::apply(Value); }

void ParenExpr::setValue(ExprAST *E) { SetterImpl(Value, E); }
UniquePtrToAST ParenExpr::getValue() &&{ return RvalueGetterImpl(Value); }

BoolOpExpr::~BoolOpExpr() { DeleteAST::apply(Value); }

void BoolOpExpr::setValue(ExprAST *E) {
  SetterImpl(Value, E);
  setHasCompareOp(IsInstance<BinOpExpr>(E) &&
      isCompareOp(static_cast<BinOpExpr *>(E)->getOp()));
}

bool BoolOpExpr::isCompareOp(BinaryOpKind Op) {
  switch (Op) {
  default:return false;
#define HANDLE_COMPARE_OPERATOR(VAL, OP, FUNC)                                 \
  case BinaryOpKind::VAL:                                                      \
    return true;
#include "simplecc/Parse/Enums.def"
  }
}

UniquePtrToAST BoolOpExpr::getValue() &&{ return RvalueGetterImpl(Value); }

UnaryOpExpr::~UnaryOpExpr() { DeleteAST::apply(Operand); }

void UnaryOpExpr::setOperand(ExprAST *E) { SetterImpl(Operand, E); }

UniquePtrToAST UnaryOpExpr::getOperand() &&{
  return RvalueGetterImpl(Operand);
}

CallExpr::~CallExpr() { DeleteAST::apply(Args); }

void CallExpr::setArgAt(unsigned I, ExprAST *Val) { SetterImpl(Args[I], Val); }

SubscriptExpr::~SubscriptExpr() { DeleteAST::apply(Index); }

void SubscriptExpr::setIndex(ExprAST *E) { SetterImpl(Index, E); }

UniquePtrToAST SubscriptExpr::getIndex() &&{ return RvalueGetterImpl(Index); }

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
