#ifndef VISITOR_H
#define VISITOR_H
#include "AST.h"

// Visitor Mixin that provides runtime dispatch of abstract nodes
template <typename Derived> class VisitorBase {
public:
  template <typename R, typename... Args>
  R visitDecl(Decl *node, Args &&... args) {

    if (auto x = subclass_cast<ConstDecl>(node)) {
      return static_cast<Derived *>(this)->visitConstDecl(
          x, std::forward<Args>(args)...);
    }

    if (auto x = subclass_cast<VarDecl>(node)) {
      return static_cast<Derived *>(this)->visitVarDecl(
          x, std::forward<Args>(args)...);
    }

    if (auto x = subclass_cast<FuncDef>(node)) {
      return static_cast<Derived *>(this)->visitFuncDef(
          x, std::forward<Args>(args)...);
    }
    assert(false && "Decl");
  }

  template <typename R, typename... Args>
  R visitStmt(Stmt *node, Args &&... args) {

    if (auto x = subclass_cast<Read>(node)) {
      return static_cast<Derived *>(this)->visitRead(
          x, std::forward<Args>(args)...);
    }

    if (auto x = subclass_cast<Write>(node)) {
      return static_cast<Derived *>(this)->visitWrite(
          x, std::forward<Args>(args)...);
    }

    if (auto x = subclass_cast<Assign>(node)) {
      return static_cast<Derived *>(this)->visitAssign(
          x, std::forward<Args>(args)...);
    }

    if (auto x = subclass_cast<For>(node)) {
      return static_cast<Derived *>(this)->visitFor(
          x, std::forward<Args>(args)...);
    }

    if (auto x = subclass_cast<While>(node)) {
      return static_cast<Derived *>(this)->visitWhile(
          x, std::forward<Args>(args)...);
    }

    if (auto x = subclass_cast<Return>(node)) {
      return static_cast<Derived *>(this)->visitReturn(
          x, std::forward<Args>(args)...);
    }

    if (auto x = subclass_cast<If>(node)) {
      return static_cast<Derived *>(this)->visitIf(x,
                                                   std::forward<Args>(args)...);
    }

    if (auto x = subclass_cast<ExprStmt>(node)) {
      return static_cast<Derived *>(this)->visitExprStmt(
          x, std::forward<Args>(args)...);
    }
    assert(false && "Stmt");
  }

  template <typename R, typename... Args>
  R visitExpr(Expr *node, Args &&... args) {

    if (auto x = subclass_cast<BinOp>(node)) {
      return static_cast<Derived *>(this)->visitBinOp(
          x, std::forward<Args>(args)...);
    }

    if (auto x = subclass_cast<ParenExpr>(node)) {
      return static_cast<Derived *>(this)->visitParenExpr(
          x, std::forward<Args>(args)...);
    }

    if (auto x = subclass_cast<BoolOp>(node)) {
      return static_cast<Derived *>(this)->visitBoolOp(
          x, std::forward<Args>(args)...);
    }

    if (auto x = subclass_cast<UnaryOp>(node)) {
      return static_cast<Derived *>(this)->visitUnaryOp(
          x, std::forward<Args>(args)...);
    }

    if (auto x = subclass_cast<Call>(node)) {
      return static_cast<Derived *>(this)->visitCall(
          x, std::forward<Args>(args)...);
    }

    if (auto x = subclass_cast<Num>(node)) {
      return static_cast<Derived *>(this)->visitNum(
          x, std::forward<Args>(args)...);
    }

    if (auto x = subclass_cast<Str>(node)) {
      return static_cast<Derived *>(this)->visitStr(
          x, std::forward<Args>(args)...);
    }

    if (auto x = subclass_cast<Char>(node)) {
      return static_cast<Derived *>(this)->visitChar(
          x, std::forward<Args>(args)...);
    }

    if (auto x = subclass_cast<Subscript>(node)) {
      return static_cast<Derived *>(this)->visitSubscript(
          x, std::forward<Args>(args)...);
    }

    if (auto x = subclass_cast<Name>(node)) {
      return static_cast<Derived *>(this)->visitName(
          x, std::forward<Args>(args)...);
    }
    assert(false && "Expr");
  }
};

// Visitor Mixin that provides default implementation for visiting nodes
// that have children.
template <class Derived> class ChildrenVisitor {
public:
  template <typename... Args>
  void visitProgram(Program *node, Args &&... args) {

    for (auto s : node->decls) {
      visitDecl(s, args...);
    }
  }

  template <typename... Args>
  void visitConstDecl(ConstDecl *node, Args &&... args) {

    visitExpr(node->value, args...);
  }

  template <typename... Args>
  void visitVarDecl(VarDecl *node, Args &&... args) {}

  template <typename... Args>
  void visitFuncDef(FuncDef *node, Args &&... args) {

    for (auto s : node->args) {
      visitArg(s, args...);
    }

    for (auto s : node->decls) {
      visitDecl(s, args...);
    }

    for (auto s : node->stmts) {
      visitStmt(s, args...);
    }
  }

  template <typename... Args> void visitArg(Arg *node, Args &&... args) {}

  template <typename... Args> void visitRead(Read *node, Args &&... args) {

    for (auto s : node->names) {
      visitExpr(s, args...);
    }
  }

  template <typename... Args> void visitWrite(Write *node, Args &&... args) {

    if (node->str) {
      visitExpr(node->str, args...);
    }

    if (node->value) {
      visitExpr(node->value, args...);
    }
  }

  template <typename... Args> void visitAssign(Assign *node, Args &&... args) {

    visitExpr(node->target, args...);

    visitExpr(node->value, args...);
  }

  template <typename... Args> void visitFor(For *node, Args &&... args) {

    visitStmt(node->initial, args...);

    visitExpr(node->condition, args...);

    visitStmt(node->step, args...);

    for (auto s : node->body) {
      visitStmt(s, args...);
    }
  }

  template <typename... Args> void visitWhile(While *node, Args &&... args) {

    visitExpr(node->condition, args...);

    for (auto s : node->body) {
      visitStmt(s, args...);
    }
  }

  template <typename... Args> void visitReturn(Return *node, Args &&... args) {

    if (node->value) {
      visitExpr(node->value, args...);
    }
  }

  template <typename... Args> void visitIf(If *node, Args &&... args) {

    visitExpr(node->test, args...);

    for (auto s : node->body) {
      visitStmt(s, args...);
    }

    for (auto s : node->orelse) {
      visitStmt(s, args...);
    }
  }

  template <typename... Args>
  void visitExprStmt(ExprStmt *node, Args &&... args) {

    visitExpr(node->value, args...);
  }

  template <typename... Args> void visitBinOp(BinOp *node, Args &&... args) {

    visitExpr(node->left, args...);

    visitExpr(node->right, args...);
  }

  template <typename... Args>
  void visitParenExpr(ParenExpr *node, Args &&... args) {

    visitExpr(node->value, args...);
  }

  template <typename... Args> void visitBoolOp(BoolOp *node, Args &&... args) {

    visitExpr(node->value, args...);
  }

  template <typename... Args>
  void visitUnaryOp(UnaryOp *node, Args &&... args) {

    visitExpr(node->operand, args...);
  }

  template <typename... Args> void visitCall(Call *node, Args &&... args) {

    for (auto s : node->args) {
      visitExpr(s, args...);
    }
  }

  template <typename... Args> void visitNum(Num *node, Args &&... args) {}

  template <typename... Args> void visitStr(Str *node, Args &&... args) {}

  template <typename... Args> void visitChar(Char *node, Args &&... args) {}

  template <typename... Args>
  void visitSubscript(Subscript *node, Args &&... args) {

    visitExpr(node->index, args...);
  }

  template <typename... Args> void visitName(Name *node, Args &&... args) {}

  template <typename... Args> void visitDecl(Decl *node, Args &&... args) {
    static_cast<Derived *>(this)->visitDecl(node, std::forward<Args>(args)...);
  }

  template <typename... Args> void visitStmt(Stmt *node, Args &&... args) {
    static_cast<Derived *>(this)->visitStmt(node, std::forward<Args>(args)...);
  }

  template <typename... Args> void visitExpr(Expr *node, Args &&... args) {
    static_cast<Derived *>(this)->visitExpr(node, std::forward<Args>(args)...);
  }
};

#endif