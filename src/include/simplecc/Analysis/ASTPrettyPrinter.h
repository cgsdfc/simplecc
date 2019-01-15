#ifndef SIMPLECC_ANALYSIS_ASTPRETTYPRINTER_H
#define SIMPLECC_ANALYSIS_ASTPRETTYPRINTER_H
#include "simplecc/Analysis/Visitor.h"
#include "simplecc/Support/IndentAwarePrinter.h"

namespace simplecc {
/// ASTPrettyPrinter implements the pretty-printing of the AST.
/// The pretty-printing tries to visualize an AST textually to aid debugging
/// or simply feel cool. The format basically follows:
/// 1. Class name of the node.
/// 2. Short attribute like name or type.
/// 3. Short print-out of children that stay in the same line.
/// 4. Children nodes with optionally keywords description, e.g., ``LHS=...``
///
/// Note: Location is not included as it tends to make output verbose.
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

  /// Print a list of statements.
  /// Wrapped in brackets and separated by commas.
  void printStmtList(const StmtAST::StmtListType &StmtList);
  void printArgs(const std::vector<ArgDecl *> &Args);

  /// Return if an expression is atomic, i.e., has no children.
  bool isAtomicExpr(ExprAST *E) const;
  /// Return if a CallExpr passes no argument.
  bool hasNoArgument(CallExpr *C) const { return !C->getNumArgs(); }

public:
  explicit ASTPrettyPrinter(std::ostream &O) : OS(O) {}
  /// Pretty print an AST node.
  void PrettyPrint(const AST *A);

private:
  friend IndentAwarePrinter;
  friend VisitorBase;
  std::ostream &OS;
  /// Required by IndentAwarePrinter.
  std::ostream &getOS() { return OS; }
};
} // namespace simplecc

#endif // SIMPLECC_ANALYSIS_ASTPRETTYPRINTER_H