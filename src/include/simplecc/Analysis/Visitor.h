#ifndef VISITOR_H
#define VISITOR_H
#include "simplecc/Parse/AST.h"
#include <cassert>

namespace simplecc {

// Visitor Mixin that provides runtime dispatch of abstract nodes
template<typename Derived> class VisitorBase {
public:
  template<typename RetTy = void> RetTy visitDecl(Decl *D);
  template<typename RetTy = void> RetTy visitStmt(Stmt *S);
  template<typename RetTy = void> RetTy visitExpr(Expr *E);
};

/// Visitor that simply visits children recursively and return no value.
template<class Derived> class ChildrenVisitor : public VisitorBase<Derived> {
public:
  /// Decl subclasses.
  void visitProgram(Program *node);
  void visitConstDecl(ConstDecl *node);
  void visitVarDecl(VarDecl *node) {}
  void visitFuncDef(FuncDef *node);
  void visitArgDecl(ArgDecl *node) {}

  /// Stmt subclasses.
  void visitRead(Read *node);
  void visitWrite(Write *node);
  void visitAssign(Assign *node);
  void visitFor(For *node);
  void visitWhile(While *node);
  void visitReturn(Return *node);
  void visitIf(If *node);
  void visitExprStmt(ExprStmt *node);

  // Expr subclasses.
  void visitBinOp(BinOp *node);
  void visitParenExpr(ParenExpr *node);
  void visitBoolOp(BoolOp *node);
  void visitUnaryOp(UnaryOp *node);
  void visitCall(Call *node);
  void visitNum(Num *node) {}
  void visitStr(Str *node) {}
  void visitChar(Char *node) {}
  void visitSubscript(Subscript *node);
  void visitName(Name *node) {}

  /// Pull in VisitorBase's methods.
  using VisitorBase<Derived>::visitDecl;
  using VisitorBase<Derived>::visitStmt;
  using VisitorBase<Derived>::visitExpr;
};

// Methods of VisitorBase.
template<typename Derived>
template<typename RetTy>
RetTy VisitorBase<Derived>::visitDecl(Decl *D) {
  if (auto x = subclass_cast<ConstDecl>(D)) {
    return static_cast<Derived *>(this)->visitConstDecl(x);
  }

  if (auto x = subclass_cast<VarDecl>(D)) {
    return static_cast<Derived *>(this)->visitVarDecl(x);
  }

  if (auto x = subclass_cast<FuncDef>(D)) {
    return static_cast<Derived *>(this)->visitFuncDef(x);
  }

  if (auto x = subclass_cast<ArgDecl>(D)) {
    return static_cast<Derived *>(this)->visitArgDecl(x);
  }
  assert(false && "Unhandled Decl subclasses");
}

template<typename Derived>
template<typename RetTy>
RetTy VisitorBase<Derived>::visitStmt(Stmt *S) {
  if (auto x = subclass_cast<Read>(S)) {
    return static_cast<Derived *>(this)->visitRead(x);
  }

  if (auto x = subclass_cast<Write>(S)) {
    return static_cast<Derived *>(this)->visitWrite(x);
  }

  if (auto x = subclass_cast<Assign>(S)) {
    return static_cast<Derived *>(this)->visitAssign(x);
  }

  if (auto x = subclass_cast<For>(S)) {
    return static_cast<Derived *>(this)->visitFor(x);
  }

  if (auto x = subclass_cast<While>(S)) {
    return static_cast<Derived *>(this)->visitWhile(x);
  }

  if (auto x = subclass_cast<Return>(S)) {
    return static_cast<Derived *>(this)->visitReturn(x);
  }

  if (auto x = subclass_cast<If>(S)) {
    return static_cast<Derived *>(this)->visitIf(x);
  }

  if (auto x = subclass_cast<ExprStmt>(S)) {
    return static_cast<Derived *>(this)->visitExprStmt(x);
  }
  assert(false && "Unhandled Stmt subclasses");
}

template<typename Derived>
template<typename RetTy>
RetTy VisitorBase<Derived>::visitExpr(Expr *E) {
  if (auto x = subclass_cast<BinOp>(E)) {
    return static_cast<Derived *>(this)->visitBinOp(x);
  }

  if (auto x = subclass_cast<ParenExpr>(E)) {
    return static_cast<Derived *>(this)->visitParenExpr(x);
  }

  if (auto x = subclass_cast<BoolOp>(E)) {
    return static_cast<Derived *>(this)->visitBoolOp(x);
  }

  if (auto x = subclass_cast<UnaryOp>(E)) {
    return static_cast<Derived *>(this)->visitUnaryOp(x);
  }

  if (auto x = subclass_cast<Call>(E)) {
    return static_cast<Derived *>(this)->visitCall(x);
  }

  if (auto x = subclass_cast<Num>(E)) {
    return static_cast<Derived *>(this)->visitNum(x);
  }

  if (auto x = subclass_cast<Str>(E)) {
    return static_cast<Derived *>(this)->visitStr(x);
  }

  if (auto x = subclass_cast<Char>(E)) {
    return static_cast<Derived *>(this)->visitChar(x);
  }

  if (auto x = subclass_cast<Subscript>(E)) {
    return static_cast<Derived *>(this)->visitSubscript(x);
  }

  if (auto x = subclass_cast<Name>(E)) {
    return static_cast<Derived *>(this)->visitName(x);
  }
  assert(false && "Unhandled Expr subclasses");
}

// Methods of ChildrenVisitor
template<class Derived>
void ChildrenVisitor<Derived>::visitProgram(Program *node) {
  for (auto s : node->getDecls()) {
    static_cast<Derived *>(this)->visitDecl(s);
  }
}

template<class Derived>
void ChildrenVisitor<Derived>::visitConstDecl(ConstDecl *node) {
  static_cast<Derived *>(this)->visitExpr(node->getValue());
}

template<class Derived>
void ChildrenVisitor<Derived>::visitFuncDef(FuncDef *node) {
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

template<class Derived>
void ChildrenVisitor<Derived>::visitRead(Read *node) {

  for (auto s : node->getNames()) {
    static_cast<Derived *>(this)->visitExpr(s);
  }
}

template<class Derived>
void ChildrenVisitor<Derived>::visitWrite(Write *node) {

  if (auto s = node->getStr()) {
    static_cast<Derived *>(this)->visitExpr(s);
  }

  if (auto s = node->getValue()) {
    static_cast<Derived *>(this)->visitExpr(s);
  }
}

template<class Derived>
void ChildrenVisitor<Derived>::visitAssign(Assign *node) {

  static_cast<Derived *>(this)->visitExpr(node->getTarget());

  static_cast<Derived *>(this)->visitExpr(node->getValue());
}

template<class Derived>
void ChildrenVisitor<Derived>::visitFor(For *node) {
  static_cast<Derived *>(this)->visitStmt(node->getInitial());
  static_cast<Derived *>(this)->visitExpr(node->getCondition());
  static_cast<Derived *>(this)->visitStmt(node->getStep());

  for (auto s : node->getBody()) {
    static_cast<Derived *>(this)->visitStmt(s);
  }
}

template<class Derived>
void ChildrenVisitor<Derived>::visitWhile(While *node) {
  static_cast<Derived *>(this)->visitExpr(node->getCondition());

  for (auto s : node->getBody()) {
    static_cast<Derived *>(this)->visitStmt(s);
  }
}

template<class Derived>
void ChildrenVisitor<Derived>::visitReturn(Return *node) {

  if (auto s = node->getValue()) {
    static_cast<Derived *>(this)->visitExpr(s);
  }
}

template<class Derived>
void ChildrenVisitor<Derived>::visitIf(If *node) {

  static_cast<Derived *>(this)->visitExpr(node->getTest());

  for (auto s : node->getBody()) {
    static_cast<Derived *>(this)->visitStmt(s);
  }

  for (auto s : node->getOrelse()) {
    static_cast<Derived *>(this)->visitStmt(s);
  }
}

template<class Derived>
void ChildrenVisitor<Derived>::visitExprStmt(ExprStmt *node) {

  static_cast<Derived *>(this)->visitExpr(node->getValue());
}

template<class Derived>
void ChildrenVisitor<Derived>::visitBinOp(BinOp *node) {

  static_cast<Derived *>(this)->visitExpr(node->getLeft());

  static_cast<Derived *>(this)->visitExpr(node->getRight());
}

template<class Derived>
void ChildrenVisitor<Derived>::visitParenExpr(ParenExpr *node) {

  static_cast<Derived *>(this)->visitExpr(node->getValue());
}

template<class Derived>
void ChildrenVisitor<Derived>::visitBoolOp(BoolOp *node) {

  static_cast<Derived *>(this)->visitExpr(node->getValue());
}

template<class Derived>
void ChildrenVisitor<Derived>::visitUnaryOp(UnaryOp *node) {

  static_cast<Derived *>(this)->visitExpr(node->getOperand());
}

template<class Derived>
void ChildrenVisitor<Derived>::visitCall(Call *node) {

  for (auto s : node->getArgs()) {
    static_cast<Derived *>(this)->visitExpr(s);
  }
}

template<class Derived>
void ChildrenVisitor<Derived>::visitSubscript(Subscript *node) {

  static_cast<Derived *>(this)->visitExpr(node->getIndex());
}

} // namespace simplecc
#endif