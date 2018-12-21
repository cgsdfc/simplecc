#ifndef SIMPLECOMPILER_DESCRIPTIONVISITOR_H
#define SIMPLECOMPILER_DESCRIPTIONVISITOR_H
#include "simplecc/Analysis/Visitor.h"

namespace simplecc {
class AstRef;

/// This class generates a description for each AST node.
class DescriptionVisitor : public VisitorBase<DescriptionVisitor> {
  friend class VisitorBase<DescriptionVisitor>;

  /// VisitorBase boilerplate code.
  String visitDecl(Decl *D) { return VisitorBase::visitDecl<String>(D); }
  String visitExpr(Expr *E) { return VisitorBase::visitExpr<String>(E); }
  String visitStmt(Stmt *S) { return VisitorBase::visitStmt<String>(S); }

  String visitConstDecl(ConstDecl *CD);
  String visitVarDecl(VarDecl *VD);
  String visitFuncDef(FuncDef *FD);
  String visitArgDecl(ArgDecl *A);

  String visitBoolOp(BoolOp *BO) { return ""; }
  String visitParenExpr(ParenExpr *PE) { return "()"; }

  String visitNum(Num *N);

  String visitChar(Char *C);
  String visitStr(Str *S);

  String visitBinOp(BinOp *BO) { return CStringFromOperatorKind(BO->getOp()); }
  String visitUnaryOp(UnaryOp *UO) {
    return CStringFromUnaryopKind(UO->getOp());
  }
  String visitCall(Call *C) { return C->getFunc(); }
  String visitName(Name *N) { return N->getId(); }
  String visitRead(Read *) { return "scanf"; }
  String visitWrite(Write *) { return "printf"; }
  String visitAssign(Assign *) { return "="; }
  String visitSubscript(Subscript *SB) { return SB->getName(); }
  String visitExprStmt(ExprStmt *ES) { return ""; }
  String visitFor(For *) { return ""; }
  String visitIf(If *) { return ""; }
  String visitWhile(While *) { return ""; }
  String visitReturn(Return *) { return ""; }

public:
  DescriptionVisitor() = default;

  /// Return a descriptive string for AR.
  String makeDescription(const AstRef &AR);
};

} // namespace simplecc
#endif //SIMPLECOMPILER_DESCRIPTIONVISITOR_H
