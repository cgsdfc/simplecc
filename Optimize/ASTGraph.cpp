#include "ASTGraph.h"
#include "Print.h"
#include "Visitor.h"

#include <llvm/ADT/GraphTraits.h>
#include <llvm/Support/GraphWriter.h>
#include <llvm/ADT/iterator.h>
#include <map>
#include <stack>

namespace simplecompiler {

class AstGraph;

/// Kind discriminator for direct AST subclasses.
enum class AstKind {
  Program,
  Decl,
  Stmt,
  Expr,
  Arg,
};

/// Template magic to map type to AstKind enum.
template <typename AstT> struct AstTraits {
  /// Issue an error if unspecialized AstTraits was used.
  static const AstKind Kind = AstT::UnknownAstKindError;
};

#define DEFINE_AST_TRAITS(NAME)                                                \
  template <> struct AstTraits<NAME> {                                         \
    static const AstKind Kind = AstKind::NAME;                                 \
  };

DEFINE_AST_TRAITS(Program)
DEFINE_AST_TRAITS(Decl)
DEFINE_AST_TRAITS(Stmt)
DEFINE_AST_TRAITS(Arg)
DEFINE_AST_TRAITS(Expr)
#undef DEFINE_AST_TRAITS

/// A wrapper around AST* that records its real type.
/// Stand in place of A cheap reference to Ast Nodes.
class AstRef {
  AST *Ref;
  AstKind Kind;
  /// Pointer to enclosing AstGraph. This is required since
  /// GraphTraits::child_XXX() takes **only** a NodeRef.
  AstGraph *Parent;

public:
  /// Construct from one of the known subclasses of AST.
  template <typename AstT>
  AstRef(AstT *Ptr, AstGraph *P)
      : Ref(Ptr), Kind(AstTraits<AstT>::Kind), Parent(P) {}

  /// Construct an empty AstRef, like nullptr to AST*.
  AstRef() : Ref(nullptr), Kind(), Parent() {}

  /* AstRef(const AstRef &) = delete; */

  AstKind getKind() const { return Kind; }

  bool operator==(const AstRef &O) const { return Ref == O.Ref; }

  /// Empiness test. !empty() === bool().
  bool empty() const { return nullptr == Ref; }
  operator bool() const { return !empty(); }

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

  /// Wrap AST::GetClassName().
  const char *getClassName() const {
    assert(Ref);
    return Ref->GetClassName();
  }

  /// Return the Location of the wrapped AST node.
  Location getLocation() const {
    switch (Kind) {
    default:
      return Location(0, 0);
#define DEFINE_AST_TRAITS(NAME)                                                \
  case AstKind::NAME:                                                          \
    return static_cast<NAME *>(Ref)->loc;
      DEFINE_AST_TRAITS(Decl)
      DEFINE_AST_TRAITS(Stmt)
      DEFINE_AST_TRAITS(Arg)
      DEFINE_AST_TRAITS(Expr)
#undef DEFINE_AST_TRAITS
    }
  }

  AstGraph *getParent() const { return Parent; }
};

/// Collect children of an AstRef into a vector for later use.
class ChildrenCollector : public ChildrenVisitor<ChildrenCollector> {
  /// Keep a reference to the output vector.
  std::vector<AstRef*> &Children;
  /// Used to construct an AstRef.
  AstGraph *Parent;

  /// Add a child.
  template <typename AstT> void AddChild(AstT *Ptr);

public:
  /// The type used to iterate all the children of a node.
  using ChildrenIterator =
      std::remove_reference<decltype(Children)>::type::const_iterator;

  ChildrenCollector(std::vector<AstRef*> &Vec, AstGraph *G)
      : Children(Vec), Parent(G) {}

  /// Call this to do the collecting. Otherwise, nothing will happen.
  void collect(const AstRef &R) {
    if (auto D = R.get<Decl>())
      return VisitorBase::visitDecl<void>(D);
    if (auto S = R.get<Stmt>())
      return VisitorBase::visitStmt<void>(S);
    if (auto E = R.get<Expr>())
      return VisitorBase::visitExpr<void>(E);
    if (auto P = R.get<Program>())
      return ChildrenVisitor::visitProgram(P);
    // Arg has no children
    return visitArg(R.get<Arg>());
  }

public:
  /// For ChildrenVisitor to hook in
  void visitExpr(Expr *E) { AddChild(E); }
  void visitDecl(Decl *D) { AddChild(D); }
  void visitStmt(Stmt *S) { AddChild(S); }
};


/// Implementation of AstIterator
class AstIteratorImpl {
  std::vector<AstRef*> Stack;
  AstGraph *Parent;

public:
  /// Begin Iterator.
  AstIteratorImpl(Program *Ptr, AstGraph *G);

  /// End Iterator.
  AstIteratorImpl() : Stack() {}
  /// Return the next node in the graph.
  AstRef *getNext();
};


/// Iterator to all nodes of a graph.
class AstIterator
    : private AstIteratorImpl,
      public llvm::iterator_facade_base<AstIterator, std::forward_iterator_tag,
                                        AstRef*> {

public:
  AstIterator(Program *P, AstGraph *G) : AstIteratorImpl(P, G) {
    operator++();
  }

  AstIterator() : AstIteratorImpl(), Ref() {}

  AstRef *operator*() const { return Ref; }
  bool operator==(const AstIterator &O) const { return O.Ref == Ref; }

  AstIterator &operator++() {
    Ref = getNext();
    return *this;
  }

private:
  AstRef *Ref;
};

// A class that keeps all the edges of an AstRef
class AstGraph {
public:
  AstGraph(Program &P) : Prog(&P), Edges(), Nodes() {}

  using NodeIterator = AstIterator;
  using ChildIteratorType = ChildrenCollector::ChildrenIterator;

  /// Iterate over all nodes.
  NodeIterator nodes_begin() { return NodeIterator(Prog, this); }

  NodeIterator nodes_end() { return NodeIterator(); }

  llvm::iterator_range<NodeIterator> nodes() {
    return llvm::make_range(nodes_begin(), nodes_end());
  }

  /// Iterate all children of one node.
  ChildIteratorType child_begin(AstRef R) {
    return std::begin(getEdgeOrCreate(R));
  }

  ChildIteratorType child_end(AstRef R) {
    return std::end(getEdgeOrCreate(R));
  }

  llvm::iterator_range<ChildIteratorType> children(AstRef R) {
    return llvm::make_range(child_begin(R), child_end(R));
  }

  /// For use in GraphTraits.
  static ChildIteratorType ChildBegin(AstRef N) {
    return N.getParent()->child_begin(N);
  }

  static ChildIteratorType ChildEnd(AstRef N) {
    return N.getParent()->child_end(N);
  }

  /// Lazily create edges for a Node.
  const std::vector<AstRef*> &getEdgeOrCreate(const AstRef &R) {
    auto iter = Edges.find(R.get());
    if (iter != Edges.end())
      return iter->second;

    std::vector<AstRef*> E;
    ChildrenCollector CC(E, this);
    CC.collect(R);

    auto Result = Edges.emplace(R.get(), std::move(E));
    assert(Result.second && "Emplace must succeed");
    return Result.first->second;
  }

  /// Lazily create a node, namely an AstRef
  template <typename AstT>
  AstRef *getNodeOrCreate(AstT *Ptr) {
    auto iter = Nodes.find(Ptr);
    if (iter != Nodes.end())
      return iter->second.get();
    auto Result = Nodes.emplace(Ptr, std::make_unique<AstRef>(Ptr, this));
    assert(Result.second && "Emplace must succeed");
    return Result.first->second.get();
  }

private:
  /// Root of AST.
  Program *Prog;
  /// Lazily created edges for each node.
  std::map<AST *, std::vector<AstRef*>> Edges;
  std::map<AST *, std::unique_ptr<AstRef>> Nodes;
};

AstIteratorImpl::AstIteratorImpl(Program *Ptr, AstGraph *G) : Stack(), Parent(G) {
  auto AR = Parent->getNodeOrCreate(Ptr);
  Stack.push_back(AR);
}

AstRef *AstIteratorImpl::getNext() {
  if (Stack.empty()) {
    return nullptr;
  }
  auto TOS = Stack.back();
  Stack.pop_back();
  const auto &LazyEdges = Parent->getEdgeOrCreate(*TOS);
  std::copy(LazyEdges.begin(), LazyEdges.end(), std::back_inserter(Stack));
  return TOS;
}

/// Add a child.
template <typename AstT> void ChildrenCollector::AddChild(AstT *Ptr) {
  AstRef *AR = Parent->getNodeOrCreate(Ptr);
  Children.push_back(AR);
}

void PrintAllAstNodes(Program &P, std::ostream &os) {
  AstGraph Graph(P);
  for (auto AR : Graph.nodes()) {
    Print(os, AR->getClassName(), AR->getLocation());
  }
}

/* void WriteASTGraph(Program &P, llvm::raw_ostream &os) { */
/*   AstGraph Graph(P); */
/*   llvm::WriteGraph(os, Graph); */
/* } */

} // namespace simplecompiler

namespace llvm {
using simplecompiler::AstGraph;
using simplecompiler::AstRef;

template <> struct GraphTraits<simplecompiler::AstGraph> {
  using NodeRef = AstRef;
  using nodes_iterator = AstGraph::NodeIterator;
  using ChildIteratorType = AstGraph::ChildIteratorType;

  static nodes_iterator nodes_begin(AstGraph &G) {
    return G.nodes_begin();
  }

  static nodes_iterator nodes_end(AstGraph &G) { return G.nodes_end(); }

  static ChildIteratorType child_begin(NodeRef N) {
    return AstGraph::ChildBegin(N);
  }

  static ChildIteratorType child_end(NodeRef N) {
    return AstGraph::ChildEnd(N);
  }
};

} // namespace llvm
