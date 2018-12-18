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
  void visitWrite(Write *node);
  void visitAssign(Assign *node);
  void visitFor(For *node);
  void visitWhile(While *node);
  void visitReturn(Return *node);
  void visitIf(If *node);
  void visitCall(Call *node);
  void visitBinOp(BinOp *node);
  void visitFuncDef(FuncDef *FD);

  void visitBoolOp(BoolOp *node) { TransformExpr(node->value); }
  void visitParenExpr(ParenExpr *node) { TransformExpr(node->value); }
  void visitUnaryOp(UnaryOp *node) { TransformExpr(node->operand); }
  void visitSubscript(Subscript *node) { TransformExpr(node->index); }

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
