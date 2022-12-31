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

#ifndef SIMPLECC_CODEGEN_BYTECODEPRINTER_H
#define SIMPLECC_CODEGEN_BYTECODEPRINTER_H
#include "simplecc/AST/ChildrenVisitor.h"
#include "simplecc/Support/Print.h"
#include <cassert>

namespace simplecc {
/// ExprValue handles the formatting of the result of an expression
/// It is a discriminated union of a factor Expr and a temporary.
class ExprValue {
  ExprAST *Factor;
  int Temporary;
  bool Check() const;

public:
  ExprValue() = default;
  ~ExprValue() = default;

  ExprValue(const ExprValue &) = default;
  ExprValue &operator=(const ExprValue &) = default;

  explicit ExprValue(int Temp);
  explicit ExprValue(ExprAST *Factor);

  void Format(std::ostream &O) const;
};

DEFINE_INLINE_OUTPUT_OPERATOR(ExprValue)

/// LineLabel handles the formatting of a label of two form:
/// inline form like GOTO Label_1 and non-inline form like:
/// Label_1:
/// printf t0
class LineLabel {
  /// No of this label
  unsigned No;
  /// Which form it takes
  bool IsInline = false;

public:
  explicit LineLabel(unsigned No) : No(No) {}
  LineLabel(const LineLabel &) = default;
  LineLabel &operator=(const LineLabel &) = default;

  /// Setter of inline, for use in operator<<()
  LineLabel &Inline(bool B) {
    this->IsInline = B;
    return *this;
  }
  void Format(std::ostream &O) const;
};

DEFINE_INLINE_OUTPUT_OPERATOR(LineLabel)

/// ByteCodePrinter prints a program in the form required by the school.
class ByteCodePrinter : ChildrenVisitor<ByteCodePrinter> {
  void visitConstDecl(ConstDecl *CD);
  void visitVarDecl(VarDecl *VD);
  void visitArgDecl(ArgDecl *AD);
  void visitFuncDef(FuncDef *FD);

  void visitRead(ReadStmt *RD);
  void visitWrite(WriteStmt *WR);
  void visitAssign(AssignStmt *A);
  void visitReturn(ReturnStmt *R);
  void visitFor(ForStmt *F);
  void visitIf(IfStmt *I);
  void visitWhile(WhileStmt *W);

  ExprValue visitCall(CallExpr *C);
  ExprValue visitBinOp(BinOpExpr *B);
  ExprValue visitUnaryOp(UnaryOpExpr *U);
  ExprValue visitSubscript(SubscriptExpr *SB);
  ExprValue visitExpr(ExprAST *E) {
    return ChildrenVisitor::visitExpr<ExprValue>(E);
  }
  ExprValue visitParenExpr(ParenExpr *PE) { return visitExpr(PE->getValue()); }
  ExprValue visitName(NameExpr *N) { return ExprValue(N); }
  ExprValue visitChar(CharExpr *C) { return ExprValue(C); }
  ExprValue visitNum(NumExpr *N) { return ExprValue(N); }
  ExprValue visitStr(StrExpr *S) { return ExprValue(S); }
  LineLabel CompileBoolOp(BoolOpExpr *B);

  ExprValue visitBoolOp(BoolOpExpr *) {
    assert(false && "BoolOpExpr should be handled by CompileBoolOp()");
    return ExprValue();
  }

  ExprValue MakeTemporary() { return ExprValue(TempCounter++); }
  LineLabel MakeLineLabel() { return LineLabel(LabelCounter++); }
  unsigned getTempCounter() const { return TempCounter; }

public:
  explicit ByteCodePrinter(std::ostream &O) : w(O) {}
  ~ByteCodePrinter() = default;

  void PrintByteCode(ProgramAST *P) { visitProgram(P); }

private:
  friend ChildrenVisitor;
  friend VisitorBase;

  Printer w;
  unsigned TempCounter = 0;
  unsigned LabelCounter = 0;
};

} // namespace simplecc
#endif
