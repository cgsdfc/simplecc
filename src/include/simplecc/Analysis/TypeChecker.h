#ifndef SIMPLECC_ANALYSIS_TYPECHECKER_H
#define SIMPLECC_ANALYSIS_TYPECHECKER_H
#include "simplecc/Analysis/AnalysisVisitor.h"

namespace simplecc {
class TypeChecker : AnalysisVisitor<TypeChecker> {
  void visitRead(ReadStmt *RD);
  void visitWrite(WriteStmt *WR);
  void visitReturn(ReturnStmt *R);
  void visitAssign(AssignStmt *A);

  // check the operand of BoolOpExpr, restrict to int
  void CheckBoolOpOperand(ExprAST *E);
  BasicTypeKind visitBoolOp(BoolOpExpr *B);

  // check the operand of ExprAST, restrict to NOT void
  BasicTypeKind CheckExprOperand(ExprAST *E);
  BasicTypeKind visitBinOp(BinOpExpr *B);
  BasicTypeKind visitUnaryOp(UnaryOpExpr *U);
  BasicTypeKind visitParenExpr(ParenExpr *PE);
  BasicTypeKind visitCall(CallExpr *C);
  BasicTypeKind visitSubscript(SubscriptExpr *SB);
  BasicTypeKind visitName(NameExpr *N);

  // Return the type of evaluating the expression.
  BasicTypeKind visitExpr(ExprAST *E);

  BasicTypeKind visitNum(NumExpr *) { return BasicTypeKind::Int; }
  BasicTypeKind visitChar(CharExpr *) { return BasicTypeKind::Character; }

  void visitFuncDef(FuncDef *FD);
  void setFuncDef(FuncDef *FD) { TheFuncDef = FD; }

  // not actually used, for instantiation only.
  BasicTypeKind visitStr(StrExpr *);
public:
  TypeChecker() : AnalysisVisitor("TypeError") {}
  using AnalysisVisitor::Check;
private:
  friend AnalysisVisitor;
  friend ChildrenVisitor;
  friend VisitorBase;
  FuncDef *TheFuncDef;
};
} // namespace simplecc
#endif
