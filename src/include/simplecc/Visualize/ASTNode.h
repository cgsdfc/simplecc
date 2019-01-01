#ifndef SIMPLECC_VISUALIZE_ASTREF_H
#define SIMPLECC_VISUALIZE_ASTREF_H

namespace simplecc {
class ASTGraph;
class AST;

/// A wrapper class that binds an ASTGraph to an AST ptr.
class ASTNode {
  AST *Ref;
  /// Pointer to enclosing ASTGraph. This is required since
  /// GraphTraits::child_XXX() takes **only** a NodeRef.
  ASTGraph *Parent;
public:
  ASTNode(AST *Ptr, ASTGraph *P) : Ref(Ptr), Parent(P) {}

  AST *get() const { return Ref; }
  ASTGraph *getParent() const { return Parent; }

  const char *getClassName() const { return Ref->getClassName(); };
  Location getLocation() const { return Ref->getLocation(); }
};
} // namespace simplecc

#endif // SIMPLECC_VISUALIZE_ASTREF_H
