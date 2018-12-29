#ifndef SIMPLECC_VISUALIZE_ASTPRETTYPRINTER_H
#define SIMPLECC_VISUALIZE_ASTPRETTYPRINTER_H
#include "simplecc/Analysis/Visitor.h"
#include <iostream>

namespace simplecc {
class AstPrettyPrinter : VisitorBase<AstPrettyPrinter> {
  void visitProgram(Program *P);
  void visitFuncDef(FuncDef *FD);
  void visitConstDecl(ConstDecl *CD);
  void visitVarDecl(VarDecl *VD);
  void visitArgDecl(ArgDecl *AD);

  void visitName(NameExpr *N);
  void visitStr(StrExpr *S);
  void visitNum(NumExpr *N);
  void visitChar(CharExpr *C);

  void visitBinOp(BinOpExpr *B);
  void visitUnaryOp(UnaryOpExpr *U);
  void visitParenExpr(ParenExpr *P);
  void visitBoolOp(BoolOpExpr *B);
  void visitCall(CallExpr *C);
  void visitSubscript(SubscriptExpr *SB);

  void visitExprStmt(ExprStmt *E);
  void visitIf(IfStmt *I);
  void visitRead(ReadStmt *RD);
  void visitWrite(WriteStmt *WR);
  void visitReturn(ReturnStmt *R);
  void visitAssign(AssignStmt *A);
  void visitFor(ForStmt *F);
  void visitWhile(WhileStmt *W);

  void increaseIndentLevel() { ++IndentLevel; }
  void decreaseIndentLevel() { --IndentLevel; }
  unsigned getIndentLevel() const { return IndentLevel; }
  void printIndent();
  void printStmtList(const std::vector<Stmt *> &StmtList);
  void printArgs(const std::vector<Decl *> &Args);

  bool isAtomicExpr(Expr *E) const {
    switch (E->getKind()) {
    case Expr::NameExprKind:
    case Expr::StrExprKind:
    case Expr::NumExprKind:
    case Expr::CharExprKind:return true;
    default:return false;
    }
  }

  bool hasNoArgument(CallExpr *C) const {
    return !C->getNumArgs();
  }

public:
  AstPrettyPrinter(std::ostream &O) : OS(O) {}
  void PrettyPrint(const AST *A);
private:
  friend VisitorBase;
  unsigned IndentLevel = 0;
  std::ostream &OS;
};
}

#endif //SIMPLECC_VISUALIZE_ASTPRETTYPRINTER_H
