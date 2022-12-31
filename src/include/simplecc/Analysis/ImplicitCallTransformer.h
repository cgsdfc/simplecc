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

#ifndef SIMPLECC_ANALYSIS_IMPLICITCALLTRANSFORMER_H
#define SIMPLECC_ANALYSIS_IMPLICITCALLTRANSFORMER_H
#include "simplecc/Analysis/SymbolTable.h"
#include "simplecc/AST/ChildrenVisitor.h"

namespace simplecc {
/// @brief ImplicitCallTransformer modifies an AST and transform implicit call nodes to explicit calls.
/// What is an implicit call and why we need to transform them?
/// Q1: What is an implicit call?
/// A1: An implicit call is to call a function without parentheses.
/// It has two forms:
/// 1. ExprStmt: foo;
/// 2. Name in Expr if that Name is a function.
/// Form-1 has been handled by ASTBuilder well since this form is recognizable
/// on the grammar level. Form-2 is more tricky as it requires **type
/// information** to be recognized.
///
/// Q2: Why we need to transform them?
/// A2: Because to fully recognize form-2 we need to build a SymbolTable first
/// so this can't be done in ASTBuilder. On the other hand, to correctly perform
/// type check on the program, these implicit calls **must** be properly
/// described by the AST. Therefore the AST needs a transformation pass right
/// after the SymbolTable pass and right before the TypeChecker pass.
class ImplicitCallTransformer : ChildrenVisitor<ImplicitCallTransformer> {
  void visitWrite(WriteStmt *W);
  void visitAssign(AssignStmt *A);
  void visitFor(ForStmt *F);
  void visitWhile(WhileStmt *W);
  void visitReturn(ReturnStmt *R);
  void visitIf(IfStmt *I);

  void visitCall(CallExpr *C);
  void visitBinOp(BinOpExpr *B);
  void visitBoolOp(BoolOpExpr *B);
  void visitParenExpr(ParenExpr *PE);
  void visitUnaryOp(UnaryOpExpr *U);
  void visitSubscript(SubscriptExpr *SB);

  void visitFuncDef(FuncDef *FD);

  // TODO: use ExpressionTransformer.
  ExprAST *TransformExpr(ExprAST *E);

  /// Setters.
  void setLocalTable(LocalSymbolTable L) { TheLocalTable = L; }
  void setTable(const SymbolTable *S) { TheTable = S; }

public:
  ImplicitCallTransformer() = default;
  void Transform(ProgramAST *P, const SymbolTable &S);

private:
  friend VisitorBase;
  friend ChildrenVisitor;

  LocalSymbolTable TheLocalTable;
  const SymbolTable *TheTable;
};
} // namespace simplecc
#endif // SIMPLECC_ANALYSIS_IMPLICITCALLTRANSFORMER_H