#include "ASTGraph.h"
#include "Print.h"
#include "Visitor.h"

#include <llvm/ADT/GraphTraits.h>
#include <llvm/ADT/iterator.h>
#include <map>
#include <stack>

namespace simplecompiler {

enum class AstKind {
  Program,
  Decl,
  Stmt,
  Arg,
  Expr,
};

/// Automatically extracted AstKind
template <typename AstT> struct AstTraits {
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

class AstGraph;

class AstRef {
  AST *Ref;
  AstKind Kind;
  const AstGraph *Parent;

public:
  template <typename AstT>
  AstRef(AstT *Ptr, const AstGraph *P)
      : Ref(Ptr), Kind(AstTraits<AstT>::Kind), Parent(P) {}

  AstRef() : Ref(nullptr), Kind(), Parent() {}

  AstKind getKind() const { return Kind; }

  bool operator==(const AstRef &O) const { return Ref == O.Ref; }

  bool empty() const { return nullptr == Ref; }

  AST *get() const { return Ref; }

  operator bool() const { return !empty(); }

  /// Checked downcast
  template <typename AstT> AstT *get() const {
    if (empty())
      return nullptr;
    if (AstTraits<AstT>::Kind != getKind())
      return nullptr;
    return static_cast<AstT *>(Ref);
  }

  const char *getClassName() const {
    assert(Ref);
    return Ref->GetClassName();
  }

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

  const AstGraph *getParent() const { return Parent; }
};

/// Collect children of an AstRef into a vector for later use.
class ChildrenCollector : public ChildrenVisitor<ChildrenCollector> {
  std::vector<AstRef> &Children;
  const AstGraph *Parent;

  template <typename AstT> void AddChild(AstT *Ptr) {
    Children.emplace_back(Ptr, Parent);
  }

public:
  using ChildrenIterator =
      std::remove_reference<decltype(Children)>::type::const_iterator;
  ChildrenCollector(std::vector<AstRef> &Vec, const AstGraph *G)
      : Children(Vec), Parent(G) {}

  void collect(AstRef R) {
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

class ASTIteratorImpl : public ChildrenVisitor<ASTIteratorImpl> {
  std::vector<AstRef> Stack;
  const AstGraph *Parent;

public:
  ASTIteratorImpl(Program *Ptr, const AstGraph *G) : Stack(), Parent(G) {
    Stack.emplace_back(Ptr, G);
  }

  ASTIteratorImpl() : Stack() {}
  AstRef getNext();
};

class AstIterator
    : private ASTIteratorImpl,
      public llvm::iterator_facade_base<AstIterator, std::forward_iterator_tag,
                                        AstRef> {

public:
  AstIterator(Program *P, const AstGraph *G) : ASTIteratorImpl(P, G) {
    operator++();
  }
  AstIterator() : ASTIteratorImpl(), Ref() {}

  AstRef operator*() const { return Ref; }
  bool operator==(const AstIterator &O) const { return O.Ref == Ref; }

  AstIterator &operator++() {
    Ref = getNext();
    return *this;
  }

private:
  AstRef Ref;
};

void PrintAllAstNodes(Program *P, std::ostream &os) {
  /* for (auto AR : AstNodes(P)) { */
  /*   Print(os, AR.getClassName(), AR.getLocation()); */
  /* } */
}

// A class that keeps all the edges of an AstRef
class AstGraph {
  // For use of getEdgeOrCreate()
  friend class ASTIteratorImpl;

  const std::vector<AstRef> &getEdgeOrCreate(AstRef R) const {
    auto iter = Edges.find(R.get());
    if (iter != Edges.end())
      return iter->second;

    std::vector<AstRef> E;
    ChildrenCollector CC(E, this);
    CC.collect(R);

    auto Result = Edges.emplace(R.get(), std::move(E));
    assert(Result.second && "Emplace must succeed");
    return Result.first->second;
  }

public:
  AstGraph(Program &P) : Prog(&P), Edges() {}

  using NodeIterator = AstIterator;
  using ChildIteratorType = ChildrenCollector::ChildrenIterator;

  NodeIterator nodes_begin() const { return NodeIterator(Prog, this); }

  NodeIterator nodes_end() const { return NodeIterator(); }

  ChildIteratorType child_begin(AstRef R) const {
    return std::begin(getEdgeOrCreate(R));
  }

  ChildIteratorType child_end(AstRef R) const {
    return std::end(getEdgeOrCreate(R));
  }

private:
  Program *Prog;
  mutable std::map<AST *, std::vector<AstRef>> Edges;
};

AstRef ASTIteratorImpl::getNext() {
  if (Stack.empty()) {
    return AstRef();
  }
  auto TOS = Stack.back();
  Stack.pop_back();
  const auto &LazyEdges = Parent->getEdgeOrCreate(TOS);
  std::copy(LazyEdges.begin(), LazyEdges.end(), Stack.begin());
  return TOS;
}

} // namespace simplecompiler

namespace llvm {
using simplecompiler::AstGraph;
using simplecompiler::AstRef;

template <> struct GraphTraits<simplecompiler::AstGraph> {
  using NodeRef = AstRef;
  using nodes_iterator = AstGraph::NodeIterator;
  using ChildIteratorType = AstGraph::ChildIteratorType;

  static nodes_iterator nodes_begin(const AstGraph &G) {
    return G.nodes_begin();
  }

  static nodes_iterator nodes_end(const AstGraph &G) { return G.nodes_end(); }

  static ChildIteratorType child_begin(NodeRef N) {
    return N.getParent()->child_begin(N);
  }

  static ChildIteratorType child_end(NodeRef N) {
    return N.getParent()->child_end(N);
  }
};

} // namespace llvm
