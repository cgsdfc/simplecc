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
  void visitRead(ReadStmt *R);
  void visitWrite(WriteStmt *W);
  void visitAssign(AssignStmt *A);
  void visitFor(ForStmt *F);
  void visitWhile(WhileStmt *W);
  void visitReturn(ReturnStmt *R);
  void visitIf(IfStmt *I);
  void visitExprStmt(ExprStmt *ES);

  // Expr subclasses.
  void visitBinOp(BinOpExpr *B);
  void visitParenExpr(ParenExpr *PE);
  void visitBoolOp(BoolOpExpr *B);
  void visitUnaryOp(UnaryOpExpr *U);
  void visitCall(CallExpr *C);
  void visitNum(NumExpr *N) {}
  void visitStr(StrExpr *S) {}
  void visitChar(CharExpr *C) {}
  void visitSubscript(SubscriptExpr *SB);
  void visitName(NameExpr *N) {}

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
  if (auto x = subclass_cast<ReadStmt>(S)) {
    return static_cast<Derived *>(this)->visitRead(x);
  }
  if (auto x = subclass_cast<WriteStmt>(S)) {
    return static_cast<Derived *>(this)->visitWrite(x);
  }
  if (auto x = subclass_cast<AssignStmt>(S)) {
    return static_cast<Derived *>(this)->visitAssign(x);
  }
  if (auto x = subclass_cast<ForStmt>(S)) {
    return static_cast<Derived *>(this)->visitFor(x);
  }
  if (auto x = subclass_cast<WhileStmt>(S)) {
    return static_cast<Derived *>(this)->visitWhile(x);
  }
  if (auto x = subclass_cast<ReturnStmt>(S)) {
    return static_cast<Derived *>(this)->visitReturn(x);
  }
  if (auto x = subclass_cast<IfStmt>(S)) {
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
  if (auto x = subclass_cast<BinOpExpr>(E)) {
    return static_cast<Derived *>(this)->visitBinOp(x);
  }
  if (auto x = subclass_cast<ParenExpr>(E)) {
    return static_cast<Derived *>(this)->visitParenExpr(x);
  }
  if (auto x = subclass_cast<BoolOpExpr>(E)) {
    return static_cast<Derived *>(this)->visitBoolOp(x);
  }
  if (auto x = subclass_cast<UnaryOpExpr>(E)) {
    return static_cast<Derived *>(this)->visitUnaryOp(x);
  }
  if (auto x = subclass_cast<CallExpr>(E)) {
    return static_cast<Derived *>(this)->visitCall(x);
  }
  if (auto x = subclass_cast<NumExpr>(E)) {
    return static_cast<Derived *>(this)->visitNum(x);
  }
  if (auto x = subclass_cast<StrExpr>(E)) {
    return static_cast<Derived *>(this)->visitStr(x);
  }
  if (auto x = subclass_cast<CharExpr>(E)) {
    return static_cast<Derived *>(this)->visitChar(x);
  }
  if (auto x = subclass_cast<SubscriptExpr>(E)) {
    return static_cast<Derived *>(this)->visitSubscript(x);
  }
  if (auto x = subclass_cast<NameExpr>(E)) {
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

template<class Derived> void ChildrenVisitor<Derived>::visitRead(ReadStmt *R) {
  for (auto s : R->getNames()) {
    static_cast<Derived *>(this)->visitExpr(s);
  }
}

template<class Derived> void ChildrenVisitor<Derived>::visitWrite(WriteStmt *W) {
  if (auto s = W->getStr()) {
    static_cast<Derived *>(this)->visitExpr(s);
  }
  if (auto s = W->getValue()) {
    static_cast<Derived *>(this)->visitExpr(s);
  }
}

template<class Derived> void ChildrenVisitor<Derived>::visitAssign(AssignStmt *A) {
  static_cast<Derived *>(this)->visitExpr(A->getTarget());
  static_cast<Derived *>(this)->visitExpr(A->getValue());
}

template<class Derived> void ChildrenVisitor<Derived>::visitFor(ForStmt *F) {
  static_cast<Derived *>(this)->visitStmt(F->getInitial());
  static_cast<Derived *>(this)->visitExpr(F->getCondition());
  static_cast<Derived *>(this)->visitStmt(F->getStep());
  for (auto s : F->getBody()) {
    static_cast<Derived *>(this)->visitStmt(s);
  }
}

template<class Derived> void ChildrenVisitor<Derived>::visitWhile(WhileStmt *W) {
  static_cast<Derived *>(this)->visitExpr(W->getCondition());
  for (auto s : W->getBody()) {
    static_cast<Derived *>(this)->visitStmt(s);
  }
}

template<class Derived> void ChildrenVisitor<Derived>::visitReturn(ReturnStmt *R) {
  if (auto s = R->getValue()) {
    static_cast<Derived *>(this)->visitExpr(s);
  }
}

template<class Derived> void ChildrenVisitor<Derived>::visitIf(IfStmt *I) {
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

template<class Derived> void ChildrenVisitor<Derived>::visitBinOp(BinOpExpr *B) {
  static_cast<Derived *>(this)->visitExpr(B->getLeft());
  static_cast<Derived *>(this)->visitExpr(B->getRight());
}

template <class Derived>
void ChildrenVisitor<Derived>::visitParenExpr(ParenExpr *PE) {
  static_cast<Derived *>(this)->visitExpr(PE->getValue());
}

template<class Derived> void ChildrenVisitor<Derived>::visitBoolOp(BoolOpExpr *B) {
  static_cast<Derived *>(this)->visitExpr(B->getValue());
}

template <class Derived>
void ChildrenVisitor<Derived>::visitUnaryOp(UnaryOpExpr *U) {
  static_cast<Derived *>(this)->visitExpr(U->getOperand());
}

template<class Derived> void ChildrenVisitor<Derived>::visitCall(CallExpr *C) {
  for (auto s : C->getArgs()) {
    static_cast<Derived *>(this)->visitExpr(s);
  }
}

template <class Derived>
void ChildrenVisitor<Derived>::visitSubscript(SubscriptExpr *SB) {
  static_cast<Derived *>(this)->visitExpr(SB->getIndex());
}

} // namespace simplecc
#endif