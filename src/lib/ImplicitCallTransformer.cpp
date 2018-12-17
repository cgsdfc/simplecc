// XXX: Since this pass **modifies** the AST, it is important
// to run a verify pass on it to ensure it didn't break any invariant.
#include "simplecompiler/ImplicitCallTransformer.h"
#include "simplecompiler/SymbolTable.h"
#include "simplecompiler/Visitor.h"

#include <cassert>

namespace simplecompiler {

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
/// Because to fully recognize form-2 we need to build a SymbolTable first so
/// this can't be done in AstBuilder. On the other hand, to correctly perform
/// type check on the program, these implicit calls **must** be properly
/// descripted by the AST. Therefore the AST needs a transformation pass right
/// after the SymbolTable pass and right before the TypeCheker pass.
//
class ImplicitCallTransformer : ChildrenVisitor<ImplicitCallTransformer> {

  void visitWrite(Write *node) {
    if (node->getValue()) {
      TransformExpr(node->value);
    }
  }

  void visitAssign(Assign *node) {
    /// Don't transform the target!
    visitExpr(node->getTarget());
    TransformExpr(node->value);
  }

  void visitFor(For *node) {
    visitStmt(node->initial);
    TransformExpr(node->condition);
    visitStmt(node->getStep());
    for (auto s : node->getBody()) {
      visitStmt(s);
    }
  }

  void visitWhile(While *node) {
    TransformExpr(node->condition);
    for (auto s : node->getBody()) {
      visitStmt(s);
    }
  }

  void visitReturn(Return *node) {
    if (node->getValue()) {
      TransformExpr(node->value);
    }
  }

  void visitIf(If *node) {
    TransformExpr(node->test);
    for (auto s : node->getBody()) {
      visitStmt(s);
    }
    for (auto s : node->getOrelse()) {
      visitStmt(s);
    }
  }

  void visitBinOp(BinOp *node) {
    TransformExpr(node->left);
    TransformExpr(node->right);
  }

  void visitCall(Call *node) {
    for (int I = 0, Size = node->getArgs().size(); I < Size; I++) {
      TransformExpr(node->args[I]);
    }
  }

  void visitFuncDef(FuncDef *FD) {
    setLocalTable(TheTable->getLocalTable(FD));
    ChildrenVisitor::visitFuncDef(FD);
  }

  void visitBoolOp(BoolOp *node) { TransformExpr(node->value); }
  void visitParenExpr(ParenExpr *node) { TransformExpr(node->value); }
  void visitUnaryOp(UnaryOp *node) { TransformExpr(node->operand); }
  void visitSubscript(Subscript *node) { TransformExpr(node->index); }

  /// Setters.
  void setLocalTable(SymbolTableView L) { TheLocalTable = L; }
  void setTable(const SymbolTable *S) { TheTable = S; }

  /// Visit an Expr and do implicit call transform on it if applicable.
  /// Note: This is deliberately not named after visitExpr() to prevent
  /// ChildrenVisitor accidentally call it, which may lead to unexpected
  /// result. For example, we don't override visitRead() since it **does not**
  /// need transformation. But ChildrenVisitor will still call it and visit all
  /// its children with visitExpr() so it is important that visitExpr() don't
  /// perform any transformation.
  /// Note: This modifies its argument with reference!
  void TransformExpr(Expr *&E) {
    if (!IsInstance<Name>(E))
      return visitExpr(E);
    if (Name *N = static_cast<Name *>(E);
        !TheLocalTable[N->getId()].IsFunction())
      return visitExpr(E);
    Call *C = new Call(static_cast<Name *>(E)->getId(), {}, E->getLoc());
    delete E;
    E = C;
  }

public:
  ImplicitCallTransformer() = default;

  /// Perform implicit call transform on the program using a SymbolTable.
  void Transform(Program *P, const SymbolTable &S) {
    assert(P);
    setTable(&S);
    visitProgram(P);
  }

private:
  friend class VisitorBase<ImplicitCallTransformer>;
  friend class ChildrenVisitor<ImplicitCallTransformer>;

  SymbolTableView TheLocalTable;
  const SymbolTable *TheTable;
};

void TransformImplicitCall(Program *P, const SymbolTable &S) {
  ImplicitCallTransformer().Transform(P, S);
}
} // namespace simplecompiler
