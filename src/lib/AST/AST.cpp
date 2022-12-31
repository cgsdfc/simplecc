// MIT License
// 
// Copyright (c) 2018 Cong Feng.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/// @file this file implements the AST class hierarchy.
#include "simplecc/AST/AST.h"
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

ReadStmt::ReadStmt(std::vector<NameExpr *> names, Location loc)
    : StmtAST(StmtAST::ReadStmtKind, loc), Names(std::move(names)) {}

ConstDecl::ConstDecl(BasicTypeKind type, std::string name, ExprAST *value, Location loc)
    : DeclAST(DeclAST::ConstDeclKind, type, std::move(name), loc), Value(value) {}

VarDecl::VarDecl(BasicTypeKind Type, std::string Name, bool isArray, int size, Location loc)
    : DeclAST(DeclAST::VarDeclKind, Type, std::move(Name), loc), IsArray(isArray), Size(size) {}

FuncDef::~FuncDef() {
  DeleteAST::apply(Args);
  DeleteAST::apply(Decls);
  DeleteAST::apply(Stmts);
}

FuncDef::FuncDef(BasicTypeKind return_type,
                 std::vector<ArgDecl *> args,
                 std::vector<DeclAST *> decls,
                 FuncDef::StmtListType stmts,
                 std::string name,
                 Location loc)
    : DeclAST(DeclAST::FuncDefKind, return_type, std::move(name), loc),
      Args(std::move(args)), Decls(std::move(decls)), Stmts(std::move(stmts)) {}

ArgDecl::ArgDecl(BasicTypeKind type, std::string name, Location loc)
    : DeclAST(DeclAST::ArgDeclKind, type, std::move(name), loc) {}

WriteStmt::WriteStmt(ExprAST *str, ExprAST *value, Location loc)
    : StmtAST(StmtAST::WriteStmtKind, loc), Str(str), Value(value) {}

ProgramAST::ProgramAST(std::string Filename, std::vector<DeclAST *> decls)
    : AST(ProgramASTKind, Location()), Filename(std::move(Filename)),
      Decls(std::move(decls)) {}

NameExpr::NameExpr(std::string id, ExprContextKind ctx, Location loc)
    : ExprAST(NameExprKind, loc), TheName(std::move(id)), context(ctx) {}

IfStmt::~IfStmt() {
  DeleteAST::apply(Then);
  DeleteAST::apply(Else);
}

IfStmt::IfStmt(ExprAST *C, StmtAST::StmtListType T, StmtAST::StmtListType E, Location loc)
    : StmtAST(StmtAST::IfStmtKind, loc), Cond(C), Then(std::move(T)),
      Else(std::move(E)) {}

ReturnStmt::ReturnStmt(ExprAST *value, Location loc)
    : StmtAST(StmtAST::ReturnStmtKind, loc), Value(value) {}

WhileStmt::WhileStmt(ExprAST *condition, StmtAST::StmtListType body, Location loc)
    : StmtAST(WhileStmtKind, loc), Cond(condition), Body(std::move(body)) {}

ParenExpr::ParenExpr(ExprAST *value, Location loc)
    : ExprAST(ExprAST::ParenExprKind, loc), Value(value) {}

BinOpExpr::BinOpExpr(ExprAST *left, BinaryOpKind op, ExprAST *right, Location loc)
    : ExprAST(ExprAST::BinOpExprKind, loc), Left(left), Op(op), Right(right) {}

ExprStmt::ExprStmt(ExprAST *value, Location loc)
    : StmtAST(StmtAST::ExprStmtKind, loc), Value(value) {}

ForStmt::ForStmt(StmtAST *initial, ExprAST *condition, StmtAST *step, StmtAST::StmtListType body, Location loc)
    : StmtAST(StmtAST::ForStmtKind, loc), Initial(initial),
      Cond(condition), Step(step), Body(std::move(body)) {}

void BoolOpExpr::setValue(ExprAST *E) {
  detail::setAST(Value, E);
  setHasCompareOp(IsInstance<BinOpExpr>(E) &&
      isCompareOp(static_cast<BinOpExpr *>(E)->getOp()));
}

bool BoolOpExpr::isCompareOp(BinaryOpKind Op) {
  switch (Op) {
  default:return false;
#define HANDLE_COMPARE_OPERATOR(VAL, OP, FUNC)                                 \
  case BinaryOpKind::VAL:                                                      \
    return true;
#include "simplecc/AST/Enums.def"
  }
}

SubscriptExpr::SubscriptExpr(std::string name, ExprAST *index, ExprContextKind ctx, Location loc)
    : ExprAST(SubscriptExprKind, loc),
      ArrayName(std::move(name)), Index(index), Context(ctx) {}

StrExpr::StrExpr(std::string s, Location loc)
    : ExprAST(ExprAST::StrExprKind, loc), TheStr(std::move(s)) {}

CharExpr::CharExpr(int c, Location loc)
    : ExprAST(ExprAST::CharExprKind, loc), TheChar(c) {}

NumExpr::NumExpr(int n, Location loc)
    : ExprAST(ExprAST::NumExprKind, loc), TheNum(n) {}

BoolOpExpr::BoolOpExpr(ExprAST *value, bool has_cmpop, Location loc)
    : ExprAST(BoolOpExprKind, loc), Value(value), HasCmpOp(has_cmpop) {}

void CallExpr::setArgAt(unsigned I, ExprAST *Val) { SetterImpl(Args[I], Val); }

UnaryOpExpr::UnaryOpExpr(UnaryOpKind op, ExprAST *operand, Location loc)
    : ExprAST(UnaryOpExprKind, loc), Op(op), Operand(operand) {}

CallExpr::CallExpr(std::string func, std::vector<ExprAST *> args, Location loc)
    : ExprAST(ExprAST::CallExprKind, loc),
      Callee(std::move(func)), Args(std::move(args)) {}

const char *AST::getClassName(unsigned Kind) {
  switch (Kind) {
  default:assert(false && "Unhandled AST Kind");
#define HANDLE_AST(CLASS)                                                      \
  case CLASS##Kind:                                                            \
    return #CLASS;
#include "simplecc/AST/AST.def"
  }
}

void AST::deleteAST() {
  switch (getKind()) {
  default:assert(false && "Unhandled AST Kind");
#define HANDLE_AST(CLASS)                                                      \
  case CLASS##Kind:                                                            \
    delete static_cast<CLASS *>(this);                                         \
    break;
#include "simplecc/AST/AST.def"
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
#include "simplecc/AST/AST.def"
  }
}

bool StmtAST::InstanceCheck(const AST *A) {
  switch (A->getKind()) {
  default:return false;
#define HANDLE_STMT(CLASS)                                                     \
  case AST::CLASS##Kind:                                                       \
    return true;
#include "simplecc/AST/AST.def"
  }
}

bool ExprAST::InstanceCheck(const AST *A) {
  switch (A->getKind()) {
  default:return false;
#define HANDLE_EXPR(CLASS)                                                     \
  case AST::CLASS##Kind:                                                       \
    return true;
#include "simplecc/AST/AST.def"
  }
}

int ExprAST::getConstantValue() const {
  assert(isConstant() && "getConstantValue() on non-constant!");
  switch (getKind()) {
#define HANDLE_EXPR(CLASS)                                                     \
  case ExprAST::CLASS##Kind:                                                   \
    return static_cast<const CLASS *>(this)->getConstantValueImpl();
#include "simplecc/AST/AST.def"
  default:assert(false && "Unhandled Enum Value");
  }
}

bool ExprAST::isConstant() const {
  switch (getKind()) {
#define HANDLE_EXPR(CLASS)                                                     \
  case ExprAST::CLASS##Kind:                                                   \
    return static_cast<const CLASS *>(this)->isConstantImpl();
#include "simplecc/AST/AST.def"
  default:assert(false && "Unhandled Enum Value");
  }
}