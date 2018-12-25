#ifndef SIMPLECC_VISUALIZE_ASTREF_H
#define SIMPLECC_VISUALIZE_ASTREF_H

namespace simplecc {
class AST;
class Program;
class Expr;
class Stmt;
class Decl;
class AstGraph;
class Location;

/// Kind discriminator for direct AST subclasses.
enum class AstKind {
  Program,
  Decl,
  Stmt,
  Expr,
};

/// Template magic to map type to AstKind enum.
template <typename AstT> struct AstTraits {
  static const AstKind Kind = AstT::UnknownAstKindError;
};

#define HANDLE_AST_TYPE(NAME)                                                  \
  template <> struct AstTraits<NAME> {                                         \
    static const AstKind Kind = AstKind::NAME;                                 \
  };

HANDLE_AST_TYPE(Program)
HANDLE_AST_TYPE(Decl)
HANDLE_AST_TYPE(Stmt)
HANDLE_AST_TYPE(Expr)
#undef HANDLE_AST_TYPE

/// A wrapper around AST* that records its real type.
/// Stand in place of A cheap reference to Ast Nodes.
class AstRef {
  friend class AstGraph;
  AST *Ref;
  AstKind Kind;
  /// Pointer to enclosing AstGraph. This is required since
  /// GraphTraits::child_XXX() takes **only** a NodeRef.
  AstGraph *Parent;

public:
  /// Construct from one of the known subclasses of AST.
  /// Only AstGraph is allowed to create it.
  template <typename AstT>
  AstRef(AstT *Ptr, AstGraph *P)
      : Ref(Ptr), Kind(AstTraits<AstT>::Kind), Parent(P) {}

  /// Construct an empty AstRef, like nullptr to AST*.
  AstRef() : Ref(nullptr), Kind(), Parent() {}

  AstKind getKind() const { return Kind; }

  bool operator==(const AstRef &O) const { return Ref == O.Ref; }

  /// get() without a type argument returns the **raw** AST pointer.
  AST *get() const { return Ref; }

  /// get<Type>() is like dynamic_cast<Type>() that tries to cast
  /// the raw AST* to Type* using the Kind indicator and fails back to nullptr.
  template <typename AstT> AstT *get() const {
    // non-empty most of the time.
    if (AstTraits<AstT>::Kind != getKind())
      return nullptr;
    return static_cast<AstT *>(Ref);
  }

  const char *getClassName() const;
  const Location &getLocation() const;
  AstGraph *getParent() const { return Parent; }
};
} // namespace simplecc

#endif // SIMPLECC_VISUALIZE_ASTREF_H
