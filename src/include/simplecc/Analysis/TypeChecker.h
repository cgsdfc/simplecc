#ifndef SIMPLECC_ANALYSIS_TYPECHECKER_H
#define SIMPLECC_ANALYSIS_TYPECHECKER_H
#include "simplecc/Analysis/SymbolTable.h"
#include "simplecc/Analysis/Visitor.h"
#include "simplecc/Support/ErrorManager.h"

namespace simplecc {
class TypeChecker : ChildrenVisitor<TypeChecker> {

  void visitRead(Read *RD);

  void visitWrite(Write *WR);

  void visitReturn(Return *R);

  void visitAssign(Assign *A);

  // check the operand of BoolOp, restrict to int
  void CheckBoolOpOperand(Expr *E);

  BasicTypeKind visitBoolOp(BoolOp *B);

  // check the operand of Expr, restrict to NOT void
  BasicTypeKind CheckExprOperand(Expr *E);

  BasicTypeKind visitBinOp(BinOp *B);

  BasicTypeKind visitUnaryOp(UnaryOp *U);

  BasicTypeKind visitParenExpr(ParenExpr *PE);

  BasicTypeKind visitCall(Call *C);

  BasicTypeKind visitSubscript(Subscript *SB);

  BasicTypeKind visitName(Name *N);
  // not actually used, for instantiation only
  BasicTypeKind visitStr(Str *);

  // Return the type of evaluating the expression
  BasicTypeKind visitExpr(Expr *E);

  void visitFuncDef(FuncDef *FD);

  BasicTypeKind visitNum(Num *) { return BasicTypeKind::Int; }

  BasicTypeKind visitChar(Char *) { return BasicTypeKind::Character; }

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
