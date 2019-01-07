#ifndef SIMPLECC_VISUALIZE_ASTPRETTYPRINTER_H
#define SIMPLECC_VISUALIZE_ASTPRETTYPRINTER_H
#include "simplecc/Analysis/Visitor.h"
#include <simplecc/Support/IndentAwarePrinter.h>

namespace simplecc {
class ASTPrettyPrinter : VisitorBase<ASTPrettyPrinter>,
                         IndentAwarePrinter<ASTPrettyPrinter> {
  void visitProgram(ProgramAST *P);
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

  void printStmtList(const std::vector<StmtAST *> &StmtList);
  void printArgs(const std::vector<DeclAST *> &Args);

  bool isAtomicExpr(ExprAST *E) const {
    switch (E->getKind()) {
    case ExprAST::NameExprKind:
    case ExprAST::StrExprKind:
    case ExprAST::NumExprKind:
    case ExprAST::CharExprKind:
      return true;
    default:
      return false;
    }
  }

  bool hasNoArgument(CallExpr *C) const { return !C->getNumArgs(); }

public:
  explicit ASTPrettyPrinter(std::ostream &O) : OS(O) {}
  void PrettyPrint(const AST *A);

private:
  friend IndentAwarePrinter;
  friend VisitorBase;
  std::ostream &OS;
  std::ostream &getOS() { return OS; }
};
} // namespace simplecc

#endif // SIMPLECC_VISUALIZE_ASTPRETTYPRINTER_H