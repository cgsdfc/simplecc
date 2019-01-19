#ifndef SIMPLECC_VISUALIZE_CHILDRENCOLLECTOR_H
#define SIMPLECC_VISUALIZE_CHILDRENCOLLECTOR_H
#include "simplecc/AST/Visitor.h"
#include "simplecc/Visualize/ASTGraph.h"

namespace simplecc {
class ASTNode;

/// Collect children of an ASTNode into a vector for later use.
class ChildrenCollector : ChildrenVisitor<ChildrenCollector> {
  /// Add a child.
  void AddChild(AST *Ptr) {
    ASTNode *AR = Parent->getNodeOrCreate(Ptr);
    Children.push_back(AR);
  }

  /// For ChildrenVisitor to hook in
  void visitExpr(ExprAST *E) { AddChild(E); }
  void visitDecl(DeclAST *D) { AddChild(D); }
  void visitStmt(StmtAST *S) { AddChild(S); }

public:
  ChildrenCollector(std::vector<ASTNode *> &Vec, ASTGraph *G)
      : Children(Vec), Parent(G) {}

  /// Call this to do the collecting. Otherwise, nothing will happen.
  void Collect(const ASTNode &R) {
    Children.clear();
    visitAST(R.get());
  }

private:
  friend ChildrenVisitor;
  friend VisitorBase;
  /// Keep a reference to the output vector.
  std::vector<ASTNode *> &Children;
  /// Used to construct an ASTNode.
  ASTGraph *Parent;
};
} // namespace simplecc

#endif // SIMPLECC_VISUALIZE_CHILDRENCOLLECTOR_H