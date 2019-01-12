/// @file this file implements the AST class hierarchy.
#include "simplecc/Parse/AST.h"
#include "simplecc/Support/Casting.h"

using namespace simplecc;

template <typename AstT>
static inline void SetterImpl(AstT *&LHS, AstT *RHS, bool Optional = false) {
  assert((Optional || RHS) && "Only optional field can be null");
  if (LHS == RHS)
    return;
  DeleteAST::apply(LHS);
  LHS = RHS;
}

FuncDef::~FuncDef() {
  DeleteAST::apply(Args);
  DeleteAST::apply(Decls);
  DeleteAST::apply(Stmts);
}

IfStmt::~IfStmt() {
  DeleteAST::apply(Then);
  DeleteAST::apply(Else);
}

void BoolOpExpr::setValue(ExprAST *E) {
  detail::setAST(Value, E);
  setHasCompareOp(IsInstance<BinOpExpr>(E) &&
                  isCompareOp(static_cast<BinOpExpr *>(E)->getOp()));
}

bool BoolOpExpr::isCompareOp(BinaryOpKind Op) {
  switch (Op) {
  default:
    return false;
#define HANDLE_COMPARE_OPERATOR(VAL, OP, FUNC)                                 \
  case BinaryOpKind::VAL:                                                      \
    return true;
#include "simplecc/Parse/Enums.def"
  }
}

void CallExpr::setArgAt(unsigned I, ExprAST *Val) { SetterImpl(Args[I], Val); }

const char *AST::getClassName(unsigned Kind) {
  switch (Kind) {
  default:
    assert(false && "Unhandled AST Kind");
#define HANDLE_AST(CLASS)                                                      \
  case CLASS##Kind:                                                            \
    return #CLASS;
#include "simplecc/Parse/AST.def"
  }
}

void AST::deleteAST() {
  switch (getKind()) {
  default:
    assert(false && "Unhandled AST Kind");
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
  default:
    return false;
#define HANDLE_DECL(CLASS)                                                     \
  case AST::CLASS##Kind:                                                       \
    return true;
#include "simplecc/Parse/AST.def"
  }
}

bool StmtAST::InstanceCheck(const AST *A) {
  switch (A->getKind()) {
  default:
    return false;
#define HANDLE_STMT(CLASS)                                                     \
  case AST::CLASS##Kind:                                                       \
    return true;
#include "simplecc/Parse/AST.def"
  }
}

bool ExprAST::InstanceCheck(const AST *A) {
  switch (A->getKind()) {
  default:
    return false;
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
  default:
    assert(false && "Unhandled Enum Value");
  }
}

bool ExprAST::isConstant() const {
  switch (getKind()) {
#define HANDLE_EXPR(CLASS)                                                     \
  case ExprAST::CLASS##Kind:                                                   \
    return static_cast<const CLASS *>(this)->isConstantImpl();
#include "simplecc/Parse/AST.def"
  default:
    assert(false && "Unhandled Enum Value");
  }
}