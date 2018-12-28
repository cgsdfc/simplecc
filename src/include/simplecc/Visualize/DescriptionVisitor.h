#ifndef SIMPLECC_VISUALIZE_DESCRIPTIONVISITOR_H
#define SIMPLECC_VISUALIZE_DESCRIPTIONVISITOR_H
#include "simplecc/Analysis/Visitor.h"

namespace simplecc {
class AstRef;

/// This class generates a description for each AST node.
class DescriptionVisitor : public VisitorBase<DescriptionVisitor> {
  friend class VisitorBase<DescriptionVisitor>;

  std::string visitProgram(Program *) { return ""; }

  /// VisitorBase boilerplate code.
  std::string visitDecl(Decl *D) { return VisitorBase::visitDecl<std::string>(D); }
  std::string visitExpr(Expr *E) { return VisitorBase::visitExpr<std::string>(E); }
  std::string visitStmt(Stmt *S) { return VisitorBase::visitStmt<std::string>(S); }

  std::string visitConstDecl(ConstDecl *CD);
  std::string visitVarDecl(VarDecl *VD);
  std::string visitFuncDef(FuncDef *FD);
  std::string visitArgDecl(ArgDecl *A);

  std::string visitBoolOp(BoolOpExpr *BO) { return ""; }
  std::string visitParenExpr(ParenExpr *PE) { return "()"; }

  std::string visitNum(NumExpr *N);

  std::string visitChar(CharExpr *C);
  std::string visitStr(StrExpr *S);

  std::string visitBinOp(BinOpExpr *BO) { return CStringFromOperatorKind(BO->getOp()); }
  std::string visitUnaryOp(UnaryOpExpr *UO) {
    return CStringFromUnaryopKind(UO->getOp());
  }
  std::string visitCall(CallExpr *C) { return C->getFunc(); }
  std::string visitName(NameExpr *N) { return N->getId(); }
  std::string visitRead(ReadStmt *) { return "scanf"; }
  std::string visitWrite(WriteStmt *) { return "printf"; }
  std::string visitAssign(AssignStmt *) { return "="; }
  std::string visitSubscript(SubscriptExpr *SB) { return SB->getName(); }
  std::string visitExprStmt(ExprStmt *ES) { return ""; }
  std::string visitFor(ForStmt *) { return ""; }
  std::string visitIf(IfStmt *) { return ""; }
  std::string visitWhile(WhileStmt *) { return ""; }
  std::string visitReturn(ReturnStmt *) { return ""; }

public:
  DescriptionVisitor() = default;

  /// Return a descriptive string for AR.
  std::string makeDescription(const AstRef &AR);
};

} // namespace simplecc
#endif // SIMPLECC_VISUALIZE_DESCRIPTIONVISITOR_H
