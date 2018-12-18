#ifndef IMPLICIT_CALL_TRANSFORMER_H
#define IMPLICIT_CALL_TRANSFORMER_H
#include "simplecc/SymbolTable.h"
#include "simplecc/Visitor.h"

namespace simplecc {
/// What is an implicit call and why we need to transform them?
/// Q1: What is an implicit call?
/// A1: An implicit call is to call a function without parentheses.
/// It has two forms:
/// 1. ExprStmt: foo;
/// 2. Name in Expr if that Name is a function.
/// Form-1 has been handled by AstBuilder well since this form is recognizable
/// on the grammar level. Form-2 is more tricky as it requires **type
/// information** to be recognized.
///
/// Q2: Why we need to transform them?
/// A2: Because to fully recognize form-2 we need to build a SymbolTable first so
/// this can't be done in AstBuilder. On the other hand, to correctly perform
/// type check on the program, these implicit calls **must** be properly
/// described by the AST. Therefore the AST needs a transformation pass right
/// after the SymbolTable pass and right before the TypeChecker pass.
//
class ImplicitCallTransformer : ChildrenVisitor<ImplicitCallTransformer> {
  void visitWrite(Write *W);
  void visitAssign(Assign *A);
  void visitFor(For *F);
  void visitWhile(While *W);
  void visitReturn(Return *R);
  void visitIf(If *I);
  void visitCall(Call *C);
  void visitBinOp(BinOp *B);
  void visitFuncDef(FuncDef *FD);

  void visitBoolOp(BoolOp *B) { TransformExpr(B->value); }
  void visitParenExpr(ParenExpr *PE) { TransformExpr(PE->value); }
  void visitUnaryOp(UnaryOp *U) { TransformExpr(U->operand); }
  void visitSubscript(Subscript *SB) { TransformExpr(SB->index); }

  void TransformExpr(Expr *&E);

  /// Setters.
  void setLocalTable(SymbolTableView L) { TheLocalTable = L; }
  void setTable(const SymbolTable *S) { TheTable = S; }

public:
  ImplicitCallTransformer() = default;
  void Transform(Program *P, const SymbolTable &S);

private:
  friend class VisitorBase<ImplicitCallTransformer>;
  friend class ChildrenVisitor<ImplicitCallTransformer>;

  SymbolTableView TheLocalTable;
  const SymbolTable *TheTable;
};

void TransformImplicitCall(Program *P, const SymbolTable &S);
} // namespace simplecc
#endif
