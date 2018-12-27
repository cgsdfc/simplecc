#ifndef SIMPLECC_VISUALIZE_DESCRIPTIONVISITOR_H
#define SIMPLECC_VISUALIZE_DESCRIPTIONVISITOR_H
#include "simplecc/Analysis/Visitor.h"

namespace simplecc {
class AstRef;

/// This class generates a description for each AST node.
class DescriptionVisitor : public VisitorBase<DescriptionVisitor> {
  friend class VisitorBase<DescriptionVisitor>;

  String visitProgram(Program *) { return ""; }

  /// VisitorBase boilerplate code.
  String visitDecl(Decl *D) { return VisitorBase::visitDecl<String>(D); }
  String visitExpr(Expr *E) { return VisitorBase::visitExpr<String>(E); }
  String visitStmt(Stmt *S) { return VisitorBase::visitStmt<String>(S); }

  String visitConstDecl(ConstDecl *CD);
  String visitVarDecl(VarDecl *VD);
  String visitFuncDef(FuncDef *FD);
  String visitArgDecl(ArgDecl *A);

  String visitBoolOp(BoolOpExpr *BO) { return ""; }
  String visitParenExpr(ParenExpr *PE) { return "()"; }

  String visitNum(NumExpr *N);

  String visitChar(CharExpr *C);
  String visitStr(StrExpr *S);

  String visitBinOp(BinOpExpr *BO) { return CStringFromOperatorKind(BO->getOp()); }
  String visitUnaryOp(UnaryOpExpr *UO) {
    return CStringFromUnaryopKind(UO->getOp());
  }
  String visitCall(CallExpr *C) { return C->getFunc(); }
  String visitName(NameExpr *N) { return N->getId(); }
  String visitRead(ReadStmt *) { return "scanf"; }
  String visitWrite(WriteStmt *) { return "printf"; }
  String visitAssign(AssignStmt *) { return "="; }
  String visitSubscript(SubscriptExpr *SB) { return SB->getName(); }
  String visitExprStmt(ExprStmt *ES) { return ""; }
  String visitFor(ForStmt *) { return ""; }
  String visitIf(IfStmt *) { return ""; }
  String visitWhile(WhileStmt *) { return ""; }
  String visitReturn(ReturnStmt *) { return ""; }

public:
  DescriptionVisitor() = default;

  /// Return a descriptive string for AR.
  String makeDescription(const AstRef &AR);
};

} // namespace simplecc
#endif // SIMPLECC_VISUALIZE_DESCRIPTIONVISITOR_H
