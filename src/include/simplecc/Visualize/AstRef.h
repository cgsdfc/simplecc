#ifndef SIMPLECC_VISUALIZE_ASTREF_H
#define SIMPLECC_VISUALIZE_ASTREF_H
#include "simplecc/Parse/AST.h"

namespace simplecc {
class AstGraph;

/// A wrapper around AST* that records its real type.
/// Stand in place of A cheap reference to Ast Nodes.
class AstRef {
  friend class AstGraph;
  AST *Ref;
  /// Pointer to enclosing AstGraph. This is required since
  /// GraphTraits::child_XXX() takes **only** a NodeRef.
  AstGraph *Parent;

public:
  /// Construct from one of the known subclasses of AST.
  /// Only AstGraph is allowed to create it.
  AstRef(AST *Ptr, AstGraph *P)
      : Ref(Ptr), Parent(P) {}

  /// Construct an empty AstRef, like nullptr to AST*.
  AstRef() : Ref(nullptr), Parent() {}

  bool operator==(const AstRef &O) const { return Ref == O.Ref; }

  /// get() without a type argument returns the **raw** AST pointer.
  AST *get() const { return Ref; }

  const char *getClassName() const { return Ref->getClassName(); };
  Location getLocation() const { return Ref->getLocation(); }
  AstGraph *getParent() const { return Parent; }
};
} // namespace simplecc

#endif // SIMPLECC_VISUALIZE_ASTREF_H
