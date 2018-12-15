// XXX: Since this pass **modifies** the AST, it is important
// to run a verify pass on it to ensure it didn't break any invariant.
#include "ImplicitCallTransformer.h"
#include "SymbolTable.h"
#include "Visitor.h"

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

/// This macro performs a visit & set on the specific member of a node.
/// It requires the visitor has its argument named "node".
/// Note: it evaluates NAME twice!
#define VISIT_AND_TRANSFORM(NAME)                                              \
  do {                                                                         \
    node->NAME = TransformExpr(node->NAME);                                    \
  } while (0)

  void visitWrite(Write *node) {
    if (node->getValue()) {
      VISIT_AND_TRANSFORM(value);
    }
  }

  void visitAssign(Assign *node) {
    visitExpr(node->getTarget());
    VISIT_AND_TRANSFORM(value);
  }

  void visitFor(For *node) {
    visitStmt(node->initial);
    VISIT_AND_TRANSFORM(condition);
    visitStmt(node->getStep());
    for (auto s : node->getBody()) {
      visitStmt(s);
    }
  }

  void visitWhile(While *node) {
    VISIT_AND_TRANSFORM(condition);
    for (auto s : node->getBody()) {
      visitStmt(s);
    }
  }

  void visitReturn(Return *node) {
    if (node->getValue()) {
      VISIT_AND_TRANSFORM(value);
    }
  }

  void visitIf(If *node) {
    VISIT_AND_TRANSFORM(test);
    for (auto s : node->getBody()) {
      visitStmt(s);
    }
    for (auto s : node->getOrelse()) {
      visitStmt(s);
    }
  }

  void visitBinOp(BinOp *node) {
    VISIT_AND_TRANSFORM(left);
    VISIT_AND_TRANSFORM(right);
  }

  /// Visit an Expr and do implicit call transform on it if applicable.
  /// Or just return the original Expr.
  /// Note: This is deliberately not named after visitExpr() to prevent
  /// ChildrenVisitor accidentally call it, which may lead to unexpected
  /// result. For example, we don't override visitRead() since it **does not**
  /// need transformation. But ChildrenVisitor will still call it and visit all
  /// its children with visitExpr() so it is important that visitExpr() don't
  /// perform any transformation.
  Expr *TransformExpr(Expr *E) {
    if (auto x = subclass_cast<Name>(E)) {
      if (TheLocalTable[x->getId()].IsFunction()) {
        // replace such a name with a call
        auto call = new Call(x->getId(), {}, x->getLoc());
        delete x;
        return call;
      } else {
        return E;
      }
    } else {
      visitExpr(E);
      return E;
    }
  }

  void visitCall(Call *node) {
    for (int I = 0, Size = node->getArgs().size(); I < Size; I++) {
      VISIT_AND_TRANSFORM(args[I]);
    }
  }

  void visitFuncDef(FuncDef *FD) {
    setLocalTable(TheTable->getLocalTable(FD));
    ChildrenVisitor::visitFuncDef(FD);
  }

  void visitBoolOp(BoolOp *node) { VISIT_AND_TRANSFORM(value); }
  void visitParenExpr(ParenExpr *node) { VISIT_AND_TRANSFORM(value); }
  void visitUnaryOp(UnaryOp *node) { VISIT_AND_TRANSFORM(operand); }
  void visitSubscript(Subscript *node) { VISIT_AND_TRANSFORM(index); }

  /// Setters.
  void setLocalTable(SymbolTableView L) { TheLocalTable = L; }
  void setTable(const SymbolTable *S) { TheTable = S; }

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
