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

#ifndef SIMPLECC_AST_CHILDRENVISITOR_H
#define SIMPLECC_AST_CHILDRENVISITOR_H
#include "simplecc/AST/VisitorBase.h"
#include <cassert>

namespace simplecc {

/// @brief ChildrenVisitor is a CRTP mixin that provides recursive visitor methods over
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
#endif // SIMPLECC_AST_CHILDRENVISITOR_H