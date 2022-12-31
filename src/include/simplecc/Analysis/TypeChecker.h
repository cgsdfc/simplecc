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

#ifndef SIMPLECC_ANALYSIS_TYPECHECKER_H
#define SIMPLECC_ANALYSIS_TYPECHECKER_H
#include "simplecc/Analysis/AnalysisVisitor.h"

namespace simplecc {
/// @brief TypeChecker implements type checking of the compiler.
/// Type checking enforces the type system, which is essentially a set of rules.
class TypeChecker : AnalysisVisitor<TypeChecker> {
  void visitRead(ReadStmt *RD);
  void visitWrite(WriteStmt *W);
  void visitReturn(ReturnStmt *R);
  void visitAssign(AssignStmt *A);

  /// check the operand of BoolOpExpr, restrict to int
  void CheckBoolOpOperand(ExprAST *E);
  BasicTypeKind visitBoolOp(BoolOpExpr *B);

  /// check the operand of ExprAST, restrict to NOT void
  BasicTypeKind CheckExprOperand(ExprAST *E);
  BasicTypeKind visitBinOp(BinOpExpr *B);
  BasicTypeKind visitUnaryOp(UnaryOpExpr *U);
  BasicTypeKind visitParenExpr(ParenExpr *PE);
  BasicTypeKind visitCall(CallExpr *C);
  BasicTypeKind visitSubscript(SubscriptExpr *SB);
  BasicTypeKind visitName(NameExpr *N);

  /// Return the type of evaluating the expression.
  BasicTypeKind visitExpr(ExprAST *E);
  BasicTypeKind visitNum(NumExpr *) { return BasicTypeKind::Int; }
  BasicTypeKind visitChar(CharExpr *) { return BasicTypeKind::Character; }

  void visitFuncDef(FuncDef *FD);
  void setFuncDef(FuncDef *FD) { TheFuncDef = FD; }

  /// not actually used, for instantiation only.
  BasicTypeKind visitStr(StrExpr *);

public:
  TypeChecker();
  /// Perform type checking.
  using AnalysisVisitor::Check;

private:
  friend AnalysisVisitor;
  friend ChildrenVisitor;
  friend VisitorBase;
  FuncDef *TheFuncDef;
};
} // namespace simplecc
#endif // SIMPLECC_ANALYSIS_TYPECHECKER_H