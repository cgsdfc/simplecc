#ifndef SIMPLECOMPILER_CHILDRENCOLLECTOR_H
#define SIMPLECOMPILER_CHILDRENCOLLECTOR_H
#include "simplecc/Analysis/Visitor.h"
#include "simplecc/Visualize/ASTGraph.h"

namespace simplecc {
class AstRef;

/// Collect children of an AstRef into a vector for later use.
class ChildrenCollector : ChildrenVisitor<ChildrenCollector> {
  /// Add a child.
  template<typename AstT> void AddChild(AstT *Ptr);

  /// For ChildrenVisitor to hook in
  void visitExpr(Expr *E) { AddChild(E); }
  void visitDecl(Decl *D) { AddChild(D); }
  void visitStmt(Stmt *S) { AddChild(S); }

public:
  ChildrenCollector(std::vector<AstRef *> &Vec, AstGraph *G)
      : Children(Vec), Parent(G) {}

  /// Call this to do the collecting. Otherwise, nothing will happen.
  void Collect(const AstRef &R);

private:
  friend class ChildrenVisitor<ChildrenCollector>;
  friend class VisitorBase<ChildrenCollector>;

  /// Keep a reference to the output vector.
  std::vector<AstRef *> &Children;
  /// Used to construct an AstRef.
  AstGraph *Parent;
};

/// Add a child.
template<typename AstT> void ChildrenCollector::AddChild(AstT *Ptr) {
  AstRef *AR = Parent->getNodeOrCreate(Ptr);
  Children.push_back(AR);
}
}

#endif //SIMPLECOMPILER_CHILDRENCOLLECTOR_H