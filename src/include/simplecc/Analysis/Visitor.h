#ifndef SIMPLECC_ANALYSIS_VISITOR_H
#define SIMPLECC_ANALYSIS_VISITOR_H
#include "simplecc/Parse/AST.h"
#include "simplecc/Support/Casting.h"
#include <cassert>

namespace simplecc {

// Visitor Mixin that provides runtime visit of abstract nodes
template <typename Derived> class VisitorBase {
public:
  template <typename RetTy = void> RetTy visitDecl(Decl *D);
  template <typename RetTy = void> RetTy visitStmt(Stmt *S);
  template <typename RetTy = void> RetTy visitExpr(Expr *E);
};

/// Visitor that simply visits children recursively and return no value.
template <class Derived> class ChildrenVisitor : public VisitorBase<Derived> {
public:
  /// Decl subclasses.
  void visitProgram(Program *P);
  void visitConstDecl(ConstDecl *CD);
  void visitVarDecl(VarDecl *VD) {}
  void visitFuncDef(FuncDef *FD);
  void visitArgDecl(ArgDecl *AD) {}

  /// Stmt subclasses.
  void visitRead(Read *R);
  void visitWrite(Write *W);
  void visitAssign(Assign *A);
  void visitFor(For *F);
  void visitWhile(While *W);
  void visitReturn(Return *R);
  void visitIf(If *I);
  void visitExprStmt(ExprStmt *ES);

  // Expr subclasses.
  void visitBinOp(BinOp *B);
  void visitParenExpr(ParenExpr *PE);
  void visitBoolOp(BoolOp *B);
  void visitUnaryOp(UnaryOp *U);
  void visitCall(Call *C);
  void visitNum(Num *N) {}
  void visitStr(Str *S) {}
  void visitChar(Char *C) {}
  void visitSubscript(Subscript *SB);
  void visitName(Name *N) {}

  /// Pull in VisitorBase's methods.
  using VisitorBase<Derived>::visitDecl;
  using VisitorBase<Derived>::visitStmt;
  using VisitorBase<Derived>::visitExpr;
};

// Methods of VisitorBase.
template <typename Derived>
template <typename RetTy>
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

template <typename Derived>
template <typename RetTy>
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

template <typename Derived>
template <typename RetTy>
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
template <class Derived>
void ChildrenVisitor<Derived>::visitProgram(Program *P) {
  for (auto s : P->getDecls()) {
    static_cast<Derived *>(this)->visitDecl(s);
  }
}

template <class Derived>
void ChildrenVisitor<Derived>::visitConstDecl(ConstDecl *CD) {
  static_cast<Derived *>(this)->visitExpr(CD->getValue());
}

template <class Derived>
void ChildrenVisitor<Derived>::visitFuncDef(FuncDef *FD) {
  for (auto s : FD->getArgs()) {
    static_cast<Derived *>(this)->visitDecl(s);
  }
  for (auto s : FD->getDecls()) {
    static_cast<Derived *>(this)->visitDecl(s);
  }
  for (auto s : FD->getStmts()) {
    static_cast<Derived *>(this)->visitStmt(s);
  }
}

template <class Derived> void ChildrenVisitor<Derived>::visitRead(Read *R) {
  for (auto s : R->getNames()) {
    static_cast<Derived *>(this)->visitExpr(s);
  }
}

template <class Derived> void ChildrenVisitor<Derived>::visitWrite(Write *W) {
  if (auto s = W->getStr()) {
    static_cast<Derived *>(this)->visitExpr(s);
  }
  if (auto s = W->getValue()) {
    static_cast<Derived *>(this)->visitExpr(s);
  }
}

template <class Derived> void ChildrenVisitor<Derived>::visitAssign(Assign *A) {
  static_cast<Derived *>(this)->visitExpr(A->getTarget());
  static_cast<Derived *>(this)->visitExpr(A->getValue());
}

template <class Derived> void ChildrenVisitor<Derived>::visitFor(For *F) {
  static_cast<Derived *>(this)->visitStmt(F->getInitial());
  static_cast<Derived *>(this)->visitExpr(F->getCondition());
  static_cast<Derived *>(this)->visitStmt(F->getStep());
  for (auto s : F->getBody()) {
    static_cast<Derived *>(this)->visitStmt(s);
  }
}

template <class Derived> void ChildrenVisitor<Derived>::visitWhile(While *W) {
  static_cast<Derived *>(this)->visitExpr(W->getCondition());
  for (auto s : W->getBody()) {
    static_cast<Derived *>(this)->visitStmt(s);
  }
}

template <class Derived> void ChildrenVisitor<Derived>::visitReturn(Return *R) {
  if (auto s = R->getValue()) {
    static_cast<Derived *>(this)->visitExpr(s);
  }
}

template <class Derived> void ChildrenVisitor<Derived>::visitIf(If *I) {
  static_cast<Derived *>(this)->visitExpr(I->getTest());
  for (auto s : I->getBody()) {
    static_cast<Derived *>(this)->visitStmt(s);
  }
  for (auto s : I->getOrelse()) {
    static_cast<Derived *>(this)->visitStmt(s);
  }
}

template <class Derived>
void ChildrenVisitor<Derived>::visitExprStmt(ExprStmt *ES) {
  static_cast<Derived *>(this)->visitExpr(ES->getValue());
}

template <class Derived> void ChildrenVisitor<Derived>::visitBinOp(BinOp *B) {
  static_cast<Derived *>(this)->visitExpr(B->getLeft());
  static_cast<Derived *>(this)->visitExpr(B->getRight());
}

template <class Derived>
void ChildrenVisitor<Derived>::visitParenExpr(ParenExpr *PE) {
  static_cast<Derived *>(this)->visitExpr(PE->getValue());
}

template <class Derived> void ChildrenVisitor<Derived>::visitBoolOp(BoolOp *B) {
  static_cast<Derived *>(this)->visitExpr(B->getValue());
}

template <class Derived>
void ChildrenVisitor<Derived>::visitUnaryOp(UnaryOp *U) {
  static_cast<Derived *>(this)->visitExpr(U->getOperand());
}

template <class Derived> void ChildrenVisitor<Derived>::visitCall(Call *C) {
  for (auto s : C->getArgs()) {
    static_cast<Derived *>(this)->visitExpr(s);
  }
}

template <class Derived>
void ChildrenVisitor<Derived>::visitSubscript(Subscript *SB) {
  static_cast<Derived *>(this)->visitExpr(SB->getIndex());
}

} // namespace simplecc
#endif