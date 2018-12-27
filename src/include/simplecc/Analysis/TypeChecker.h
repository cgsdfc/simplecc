#ifndef SIMPLECC_ANALYSIS_TYPECHECKER_H
#define SIMPLECC_ANALYSIS_TYPECHECKER_H
#include "simplecc/Analysis/SymbolTable.h"
#include "simplecc/Analysis/Visitor.h"
#include "simplecc/Support/ErrorManager.h"

namespace simplecc {
class TypeChecker : ChildrenVisitor<TypeChecker> {

  void visitRead(ReadStmt *RD);

  void visitWrite(WriteStmt *WR);

  void visitReturn(ReturnStmt *R);

  void visitAssign(AssignStmt *A);

  // check the operand of BoolOpExpr, restrict to int
  void CheckBoolOpOperand(Expr *E);

  BasicTypeKind visitBoolOp(BoolOpExpr *B);

  // check the operand of Expr, restrict to NOT void
  BasicTypeKind CheckExprOperand(Expr *E);

  BasicTypeKind visitBinOp(BinOpExpr *B);

  BasicTypeKind visitUnaryOp(UnaryOpExpr *U);

  BasicTypeKind visitParenExpr(ParenExpr *PE);

  BasicTypeKind visitCall(CallExpr *C);

  BasicTypeKind visitSubscript(SubscriptExpr *SB);

  BasicTypeKind visitName(NameExpr *N);
  // not actually used, for instantiation only
  BasicTypeKind visitStr(StrExpr *);

  // ReturnStmt the type of evaluating the expression
  BasicTypeKind visitExpr(Expr *E);

  void visitFuncDef(FuncDef *FD);

  BasicTypeKind visitNum(NumExpr *) { return BasicTypeKind::Int; }

  BasicTypeKind visitChar(CharExpr *) { return BasicTypeKind::Character; }

  void setLocalTable(SymbolTableView L) { TheLocalTable = L; }
  void setTable(SymbolTable *S) { TheTable = S; }
  void setFuncDef(FuncDef *FD) { TheFuncDef = FD; }

public:
  TypeChecker() = default;

  // public interface
  bool Check(Program *P, SymbolTable &S);

private:
  friend class ChildrenVisitor<TypeChecker>;
  friend class VisitorBase<TypeChecker>;

  SymbolTable *TheTable;
  SymbolTableView TheLocalTable;
  FuncDef *TheFuncDef;
  ErrorManager EM;
};
} // namespace simplecc
#endif
