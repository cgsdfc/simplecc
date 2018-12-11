#ifndef VISITOR_H
#define VISITOR_H
#include "AST.h"
#include <cassert>

namespace simplecompiler {

// Visitor Mixin that provides runtime dispatch of abstract nodes
template <typename Derived> class VisitorBase {
public:
  template <typename R> R visitDecl(Decl *node) {

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

  template <typename R> R visitStmt(Stmt *node) {

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

  template <typename R> R visitExpr(Expr *node) {

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
  friend class VisitorBase<Derived>;

  void visitProgram(Program *node) {

    for (auto s : node->decls) {
      visitDecl(s);
    }
  }

  void visitConstDecl(ConstDecl *node) { visitExpr(node->value); }

  void visitVarDecl(VarDecl *node) {}

  void visitFuncDef(FuncDef *node) {

    for (auto s : node->args) {
      visitDecl(s);
    }

    for (auto s : node->decls) {
      visitDecl(s);
    }

    for (auto s : node->stmts) {
      visitStmt(s);
    }
  }

  void visitArgDecl(ArgDecl *node) {}

  void visitRead(Read *node) {

    for (auto s : node->names) {
      visitExpr(s);
    }
  }

  void visitWrite(Write *node) {

    if (node->str) {
      visitExpr(node->str);
    }

    if (node->value) {
      visitExpr(node->value);
    }
  }

  void visitAssign(Assign *node) {

    visitExpr(node->target);

    visitExpr(node->value);
  }

  void visitFor(For *node) {

    visitStmt(node->initial);

    visitExpr(node->condition);

    visitStmt(node->step);

    for (auto s : node->body) {
      visitStmt(s);
    }
  }

  void visitWhile(While *node) {

    visitExpr(node->condition);

    for (auto s : node->body) {
      visitStmt(s);
    }
  }

  void visitReturn(Return *node) {

    if (node->value) {
      visitExpr(node->value);
    }
  }

  void visitIf(If *node) {

    visitExpr(node->test);

    for (auto s : node->body) {
      visitStmt(s);
    }

    for (auto s : node->orelse) {
      visitStmt(s);
    }
  }

  void visitExprStmt(ExprStmt *node) { visitExpr(node->value); }

  void visitBinOp(BinOp *node) {

    visitExpr(node->left);

    visitExpr(node->right);
  }

  void visitParenExpr(ParenExpr *node) { visitExpr(node->value); }

  void visitBoolOp(BoolOp *node) { visitExpr(node->value); }

  void visitUnaryOp(UnaryOp *node) { visitExpr(node->operand); }

  void visitCall(Call *node) {

    for (auto s : node->args) {
      visitExpr(s);
    }
  }

  void visitNum(Num *node) {}

  void visitStr(Str *node) {}

  void visitChar(Char *node) {}

  void visitSubscript(Subscript *node) { visitExpr(node->index); }

  void visitName(Name *node) {}

  void visitDecl(Decl *node) { static_cast<Derived *>(this)->visitDecl(node); }

  void visitStmt(Stmt *node) { static_cast<Derived *>(this)->visitStmt(node); }

  void visitExpr(Expr *node) { static_cast<Derived *>(this)->visitExpr(node); }
};
} // namespace simplecompiler
#endif