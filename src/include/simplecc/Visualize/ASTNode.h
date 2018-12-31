#ifndef SIMPLECC_VISUALIZE_ASTREF_H
#define SIMPLECC_VISUALIZE_ASTREF_H

namespace simplecc {
class AstGraph;
class AST;

/// A wrapper class that binds an AstGraph to an AST ptr.
class ASTNode {
  AST *Ref;
  /// Pointer to enclosing AstGraph. This is required since
  /// GraphTraits::child_XXX() takes **only** a NodeRef.
  AstGraph *Parent;
public:
  ASTNode(AST *Ptr, AstGraph *P) : Ref(Ptr), Parent(P) {}

  AST *get() const { return Ref; }
  AstGraph *getParent() const { return Parent; }

  const char *getClassName() const { return Ref->getClassName(); };
  Location getLocation() const { return Ref->getLocation(); }
};
} // namespace simplecc

#endif // SIMPLECC_VISUALIZE_ASTREF_H
