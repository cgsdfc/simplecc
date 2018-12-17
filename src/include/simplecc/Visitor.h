#ifndef VISITOR_H
#define VISITOR_H
#include "simplecc/AST.h"
#include <cassert>

namespace simplecc {

// Visitor Mixin that provides runtime dispatch of abstract nodes
template <typename Derived> class VisitorBase {
public:
  template <typename RetTy = void> RetTy visitDecl(Decl *node) {

    if (auto x = subclass_cast<ConstDecl>(node)) {
      return static_cast<Derived *>(this)->visitConstDecl(x);
    }

    if (auto x = subclass_cast<VarDecl>(node)) {
      return static_cast<Derived *>(this)->visitVarDecl(x);
    }

    if (auto x = subclass_cast<FuncDef>(node)) {
      return static_cast<Derived *>(this)->visitFuncDef(x);
    }

    if (auto x = subclass_cast<ArgDecl>(node)) {
      return static_cast<Derived *>(this)->visitArgDecl(x);
    }
    assert(false && "Decl");
  }

  template <typename RetTy = void> RetTy visitStmt(Stmt *node) {

    if (auto x = subclass_cast<Read>(node)) {
      return static_cast<Derived *>(this)->visitRead(x);
    }

    if (auto x = subclass_cast<Write>(node)) {
      return static_cast<Derived *>(this)->visitWrite(x);
    }

    if (auto x = subclass_cast<Assign>(node)) {
      return static_cast<Derived *>(this)->visitAssign(x);
    }

    if (auto x = subclass_cast<For>(node)) {
      return static_cast<Derived *>(this)->visitFor(x);
    }

    if (auto x = subclass_cast<While>(node)) {
      return static_cast<Derived *>(this)->visitWhile(x);
    }

    if (auto x = subclass_cast<Return>(node)) {
      return static_cast<Derived *>(this)->visitReturn(x);
    }

    if (auto x = subclass_cast<If>(node)) {
      return static_cast<Derived *>(this)->visitIf(x);
    }

    if (auto x = subclass_cast<ExprStmt>(node)) {
      return static_cast<Derived *>(this)->visitExprStmt(x);
    }
    assert(false && "Stmt");
  }

  template <typename RetTy = void> RetTy visitExpr(Expr *node) {

    if (auto x = subclass_cast<BinOp>(node)) {
      return static_cast<Derived *>(this)->visitBinOp(x);
    }

    if (auto x = subclass_cast<ParenExpr>(node)) {
      return static_cast<Derived *>(this)->visitParenExpr(x);
    }

    if (auto x = subclass_cast<BoolOp>(node)) {
      return static_cast<Derived *>(this)->visitBoolOp(x);
    }

    if (auto x = subclass_cast<UnaryOp>(node)) {
      return static_cast<Derived *>(this)->visitUnaryOp(x);
    }

    if (auto x = subclass_cast<Call>(node)) {
      return static_cast<Derived *>(this)->visitCall(x);
    }

    if (auto x = subclass_cast<Num>(node)) {
      return static_cast<Derived *>(this)->visitNum(x);
    }

    if (auto x = subclass_cast<Str>(node)) {
      return static_cast<Derived *>(this)->visitStr(x);
    }

    if (auto x = subclass_cast<Char>(node)) {
      return static_cast<Derived *>(this)->visitChar(x);
    }

    if (auto x = subclass_cast<Subscript>(node)) {
      return static_cast<Derived *>(this)->visitSubscript(x);
    }

    if (auto x = subclass_cast<Name>(node)) {
      return static_cast<Derived *>(this)->visitName(x);
    }
    assert(false && "Expr");
  }
};

/// Visitor that simply visits children recursively and return no value.
template <class Derived> class ChildrenVisitor : public VisitorBase<Derived> {
public:
  void visitProgram(Program *node) {

    for (auto s : node->getDecls()) {
      static_cast<Derived *>(this)->visitDecl(s);
    }
  }

  void visitConstDecl(ConstDecl *node) {

    static_cast<Derived *>(this)->visitExpr(node->getValue());
  }

  void visitVarDecl(VarDecl *node) {}

  void visitFuncDef(FuncDef *node) {

    for (auto s : node->getArgs()) {
      static_cast<Derived *>(this)->visitDecl(s);
    }

    for (auto s : node->getDecls()) {
      static_cast<Derived *>(this)->visitDecl(s);
    }

    for (auto s : node->getStmts()) {
      static_cast<Derived *>(this)->visitStmt(s);
    }
  }

  void visitArgDecl(ArgDecl *node) {}

  void visitRead(Read *node) {

    for (auto s : node->getNames()) {
      static_cast<Derived *>(this)->visitExpr(s);
    }
  }

  void visitWrite(Write *node) {

    if (auto s = node->getStr()) {
      static_cast<Derived *>(this)->visitExpr(s);
    }

    if (auto s = node->getValue()) {
      static_cast<Derived *>(this)->visitExpr(s);
    }
  }

  void visitAssign(Assign *node) {

    static_cast<Derived *>(this)->visitExpr(node->getTarget());

    static_cast<Derived *>(this)->visitExpr(node->getValue());
  }

  void visitFor(For *node) {

    static_cast<Derived *>(this)->visitStmt(node->getInitial());

    static_cast<Derived *>(this)->visitExpr(node->getCondition());

    static_cast<Derived *>(this)->visitStmt(node->getStep());

    for (auto s : node->getBody()) {
      static_cast<Derived *>(this)->visitStmt(s);
    }
  }

  void visitWhile(While *node) {

    static_cast<Derived *>(this)->visitExpr(node->getCondition());

    for (auto s : node->getBody()) {
      static_cast<Derived *>(this)->visitStmt(s);
    }
  }

  void visitReturn(Return *node) {

    if (auto s = node->getValue()) {
      static_cast<Derived *>(this)->visitExpr(s);
    }
  }

  void visitIf(If *node) {

    static_cast<Derived *>(this)->visitExpr(node->getTest());

    for (auto s : node->getBody()) {
      static_cast<Derived *>(this)->visitStmt(s);
    }

    for (auto s : node->getOrelse()) {
      static_cast<Derived *>(this)->visitStmt(s);
    }
  }

  void visitExprStmt(ExprStmt *node) {

    static_cast<Derived *>(this)->visitExpr(node->getValue());
  }

  void visitBinOp(BinOp *node) {

    static_cast<Derived *>(this)->visitExpr(node->getLeft());

    static_cast<Derived *>(this)->visitExpr(node->getRight());
  }

  void visitParenExpr(ParenExpr *node) {

    static_cast<Derived *>(this)->visitExpr(node->getValue());
  }

  void visitBoolOp(BoolOp *node) {

    static_cast<Derived *>(this)->visitExpr(node->getValue());
  }

  void visitUnaryOp(UnaryOp *node) {

    static_cast<Derived *>(this)->visitExpr(node->getOperand());
  }

  void visitCall(Call *node) {

    for (auto s : node->getArgs()) {
      static_cast<Derived *>(this)->visitExpr(s);
    }
  }

  void visitNum(Num *node) {}

  void visitStr(Str *node) {}

  void visitChar(Char *node) {}

  void visitSubscript(Subscript *node) {

    static_cast<Derived *>(this)->visitExpr(node->getIndex());
  }

  void visitName(Name *node) {}

  /// Pull in VisitorBase's methods.
  using VisitorBase<Derived>::visitDecl;
  using VisitorBase<Derived>::visitStmt;
  using VisitorBase<Derived>::visitExpr;
};
} // namespace simplecc
#endif