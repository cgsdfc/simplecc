/// @file Defines VisitorBase and ChildrenVisitor, two important visitor base classes.
#ifndef SIMPLECC_ANALYSIS_VISITOR_H
#define SIMPLECC_ANALYSIS_VISITOR_H
#include "simplecc/Parse/AST.h"
#include "simplecc/Support/Casting.h"
#include <cassert>

namespace simplecc {

/// VisitorBase is a CRTP mixin that provides dispatch over polymorphic AST nodes.
/// It implements the visitor methods for all abstract AST classes and calls the
/// visitor methods of concrete AST classes from Derived. Derived of it must implement
/// visitor methods for all concrete AST classes or the code won't compile.
///
/// The visitor methods have a template parameter for their return type, which default to ``void``.
/// This flexibility comes with some inconvenience. If you just return void, the usage is as normal.
/// But if you return some other thing, the template parameter must be provided explicitly or you can
/// wrap the explicitly parameterized method in a inline method. The latter approach is encouraged since
/// it makes code less repeated.
/// \tparam Derived Subclass that wants to visit the AST and want to implement every single visitor methods.
template <typename Derived> class VisitorBase {
public:
  /// Visit any DeclAST node.
  template <typename RetTy = void> RetTy visitDecl(DeclAST *D);
  /// Visit any StmtAST node.
  template <typename RetTy = void> RetTy visitStmt(StmtAST *S);
  /// Visit any ExprAST node.
  template <typename RetTy = void> RetTy visitExpr(ExprAST *E);
  /// Visit any AST node.
  template <typename RetTy = void> RetTy visitAST(AST *A);
};

/// ChildrenVisitor is a CRTP mixin that provides recursive visitor methods over
/// children of all AST nodes. For each child, it visits it using the visitor method from Derived,
/// which means Derived can override and customize the process. Since it inherits from VisitorBase
/// it obtains the polymorphic dispatches over abstract nodes, which serve as a well default if Derived
/// don't provide ones. Derived can even disable the visiting of a certain class by making the
/// corresponding visitor method empty. On the other hand, Derived can hook into those and performs additional
/// tasks, such as collecting nodes or resetting line number. This design makes ChildrenVisitor very customizable.
///
/// Derived don't need to implement everything. They just implement methods of interest and let ChildrenVisitor
/// recursively visits what interests Derived.
/// \tparam Derived Subclass that wants visit all the children with a certain pattern of an AST.
template <class Derived> class ChildrenVisitor : public VisitorBase<Derived> {
public:
  // Decl subclasses.
  void visitProgram(ProgramAST *P);
  void visitConstDecl(ConstDecl *CD);
  void visitFuncDef(FuncDef *FD);
  void visitVarDecl(VarDecl *VD) {}
  void visitArgDecl(ArgDecl *AD) {}

  // Stmt subclasses.
  void visitRead(ReadStmt *R);
  void visitWrite(WriteStmt *W);
  void visitAssign(AssignStmt *A);
  void visitFor(ForStmt *F);
  void visitWhile(WhileStmt *W);
  void visitReturn(ReturnStmt *R);
  void visitIf(IfStmt *I);
  void visitExprStmt(ExprStmt *ES);

  // ExprAST subclasses.
  void visitBinOp(BinOpExpr *B);
  void visitParenExpr(ParenExpr *PE);
  void visitBoolOp(BoolOpExpr *B);
  void visitUnaryOp(UnaryOpExpr *U);
  void visitSubscript(SubscriptExpr *SB);
  void visitCall(CallExpr *C);

  // Atomic ExprAST nodes.
  void visitNum(NumExpr *N) {}
  void visitStr(StrExpr *S) {}
  void visitChar(CharExpr *C) {}
  void visitName(NameExpr *N) {}

  // Pull in VisitorBase's methods as default.
  using VisitorBase<Derived>::visitDecl;
  using VisitorBase<Derived>::visitStmt;
  using VisitorBase<Derived>::visitExpr;
  using VisitorBase<Derived>::visitAST;
};

// Methods of VisitorBase.
template <typename Derived>
template <typename RetTy>
RetTy VisitorBase<Derived>::visitDecl(DeclAST *D) {
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
  assert(false && "Unhandled DeclAST subclasses");
}

template <typename Derived>
template <typename RetTy>
RetTy VisitorBase<Derived>::visitStmt(StmtAST *S) {
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
  assert(false && "Unhandled StmtAST subclasses");
}

template <typename Derived>
template <typename RetTy>
RetTy VisitorBase<Derived>::visitExpr(ExprAST *E) {
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
  assert(false && "Unhandled ExprAST subclasses");
}

template <typename Derived>
template <typename RetTy>
RetTy VisitorBase<Derived>::visitAST(AST *A) {
  if (auto x = subclass_cast<ProgramAST>(A))
    return static_cast<Derived *>(this)->visitProgram(x);
  if (auto x = subclass_cast<DeclAST>(A))
    return visitDecl<RetTy>(x);
  if (auto x = subclass_cast<StmtAST>(A))
    return visitStmt<RetTy>(x);
  if (auto x = subclass_cast<ExprAST>(A))
    return visitExpr<RetTy>(x);
  assert(false && "Unhandled AST subclasses");
}

// Methods of ChildrenVisitor
template <class Derived>
void ChildrenVisitor<Derived>::visitProgram(ProgramAST *P) {
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

template <class Derived> void ChildrenVisitor<Derived>::visitRead(ReadStmt *R) {
  for (auto s : R->getNames()) {
    static_cast<Derived *>(this)->visitExpr(s);
  }
}

template <class Derived>
void ChildrenVisitor<Derived>::visitWrite(WriteStmt *W) {
  if (auto s = W->getStr()) {
    static_cast<Derived *>(this)->visitExpr(s);
  }
  if (auto s = W->getValue()) {
    static_cast<Derived *>(this)->visitExpr(s);
  }
}

template <class Derived>
void ChildrenVisitor<Derived>::visitAssign(AssignStmt *A) {
  static_cast<Derived *>(this)->visitExpr(A->getTarget());
  static_cast<Derived *>(this)->visitExpr(A->getValue());
}

template <class Derived> void ChildrenVisitor<Derived>::visitFor(ForStmt *F) {
  static_cast<Derived *>(this)->visitStmt(F->getInitial());
  static_cast<Derived *>(this)->visitExpr(F->getCondition());
  static_cast<Derived *>(this)->visitStmt(F->getStep());
  for (auto s : F->getBody()) {
    static_cast<Derived *>(this)->visitStmt(s);
  }
}

template <class Derived>
void ChildrenVisitor<Derived>::visitWhile(WhileStmt *W) {
  static_cast<Derived *>(this)->visitExpr(W->getCondition());
  for (auto s : W->getBody()) {
    static_cast<Derived *>(this)->visitStmt(s);
  }
}

template <class Derived>
void ChildrenVisitor<Derived>::visitReturn(ReturnStmt *R) {
  if (auto s = R->getValue()) {
    static_cast<Derived *>(this)->visitExpr(s);
  }
}

template <class Derived> void ChildrenVisitor<Derived>::visitIf(IfStmt *I) {
  static_cast<Derived *>(this)->visitExpr(I->getCondition());
  for (auto s : I->getThen()) {
    static_cast<Derived *>(this)->visitStmt(s);
  }
  for (auto s : I->getElse()) {
    static_cast<Derived *>(this)->visitStmt(s);
  }
}

template <class Derived>
void ChildrenVisitor<Derived>::visitExprStmt(ExprStmt *ES) {
  static_cast<Derived *>(this)->visitExpr(ES->getValue());
}

template <class Derived>
void ChildrenVisitor<Derived>::visitBinOp(BinOpExpr *B) {
  static_cast<Derived *>(this)->visitExpr(B->getLeft());
  static_cast<Derived *>(this)->visitExpr(B->getRight());
}

template <class Derived>
void ChildrenVisitor<Derived>::visitParenExpr(ParenExpr *PE) {
  static_cast<Derived *>(this)->visitExpr(PE->getValue());
}

template <class Derived>
void ChildrenVisitor<Derived>::visitBoolOp(BoolOpExpr *B) {
  static_cast<Derived *>(this)->visitExpr(B->getValue());
}

template <class Derived>
void ChildrenVisitor<Derived>::visitUnaryOp(UnaryOpExpr *U) {
  static_cast<Derived *>(this)->visitExpr(U->getOperand());
}

template <class Derived> void ChildrenVisitor<Derived>::visitCall(CallExpr *C) {
  for (auto s : C->getArgs()) {
    static_cast<Derived *>(this)->visitExpr(s);
  }
}

template <class Derived>
void ChildrenVisitor<Derived>::visitSubscript(SubscriptExpr *SB) {
  static_cast<Derived *>(this)->visitExpr(SB->getIndex());
}

} // namespace simplecc
#endif // SIMPLECC_ANALYSIS_VISITOR_H