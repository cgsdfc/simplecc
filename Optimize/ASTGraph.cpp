#include "ASTGraph.h"
#include "Print.h"
#include "TokenInfo.h" // for Location
#include "Visitor.h"

#include <llvm/ADT/GraphTraits.h>
#include <llvm/ADT/iterator.h>
#include <llvm/Support/GraphWriter.h>

#include <algorithm>
#include <iostream>
#include <iterator> // for forward_iterator_tag
#include <map>
#include <memory> // for make_unique
#include <sstream>
#include <stack>
#include <string>
#include <type_traits> // for remove_reference
#include <utility>     // for move
#include <vector>

namespace simplecompiler {

class AstGraph;

/// Kind discriminator for direct AST subclasses.
enum class AstKind {
  Program,
  Decl,
  Stmt,
  Expr,
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
DEFINE_AST_TRAITS(Expr)
#undef DEFINE_AST_TRAITS

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

  /// Prevent accidentally having 2 AstRef pointing to the same AST
  AstRef(const AstRef &) = delete;

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
      DEFINE_AST_TRAITS(Expr)
#undef DEFINE_AST_TRAITS
    }
  }

  AstGraph *getParent() const { return Parent; }
};

/// Collect children of an AstRef into a vector for later use.
class ChildrenCollector : ChildrenVisitor<ChildrenCollector> {
  friend class ChildrenVisitor<ChildrenCollector>;
  friend class VisitorBase<ChildrenCollector>;

  /// Keep a reference to the output vector.
  std::vector<AstRef *> &Children;
  /// Used to construct an AstRef.
  AstGraph *Parent;

  /// Add a child.
  template <typename AstT> void AddChild(AstT *Ptr);

  /// For ChildrenVisitor to hook in
  void visitExpr(Expr *E) { AddChild(E); }
  void visitDecl(Decl *D) { AddChild(D); }
  void visitStmt(Stmt *S) { AddChild(S); }

public:
  /// The type used to iterate all the children of a node.
  using ChildrenIterator =
      std::remove_reference<decltype(Children)>::type::const_iterator;

  ChildrenCollector(std::vector<AstRef *> &Vec, AstGraph *G)
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
    // ArgDecl has no children
  }
};

/// Implementation of AstIterator
class AstIteratorImpl {
  std::vector<AstRef *> Stack;
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
                                        AstRef *> {

public:
  AstIterator(Program *P, AstGraph *G) : AstIteratorImpl(P, G) { operator++(); }

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
  NodeIterator nodes_begin() const {
    return NodeIterator(Prog, const_cast<AstGraph *>(this));
  }

  NodeIterator nodes_end() const { return NodeIterator(); }

  llvm::iterator_range<NodeIterator> nodes() {
    return llvm::make_range(nodes_begin(), nodes_end());
  }

  /// Iterate all children of one node.
  ChildIteratorType child_begin(const AstRef &R) {
    return std::begin(getEdgeOrCreate(R));
  }

  ChildIteratorType child_end(const AstRef &R) {
    return std::end(getEdgeOrCreate(R));
  }

  llvm::iterator_range<ChildIteratorType> children(const AstRef &R) {
    return llvm::make_range(child_begin(R), child_end(R));
  }

  /// For use in GraphTraits.
  static ChildIteratorType ChildBegin(const AstRef &N) {
    return N.getParent()->child_begin(N);
  }

  static ChildIteratorType ChildEnd(const AstRef &N) {
    return N.getParent()->child_end(N);
  }

  /// Lazily create edges for a Node.
  const std::vector<AstRef *> &getEdgeOrCreate(const AstRef &R) {
    auto iter = Edges.find(R.get());
    if (iter != Edges.end())
      return iter->second;

    std::vector<AstRef *> E;
    ChildrenCollector CC(E, this);
    CC.collect(R);

    auto Result = Edges.emplace(R.get(), std::move(E));
    assert(Result.second && "Emplace must succeed");
    return Result.first->second;
  }

  /// Lazily create a node, namely an AstRef
  template <typename AstT> AstRef *getNodeOrCreate(AstT *Ptr) {
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
  std::map<AST *, std::vector<AstRef *>> Edges;
  /// Lazily created nodes.
  std::map<AST *, std::unique_ptr<AstRef>> Nodes;
};

AstIteratorImpl::AstIteratorImpl(Program *Ptr, AstGraph *G)
    : Stack(), Parent(G) {
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

/// Print all ast nodes from a root.
void PrintAllAstNodes(Program &P, std::ostream &os) {
  AstGraph Graph(P);
  for (auto AR : Graph.nodes()) {
    Print(os, AR->getClassName(), AR->getLocation());
  }
}

/// Write an AstGraph to dot format.
void WriteASTGraph(Program &P, llvm::raw_ostream &os) {
  AstGraph Graph(P);
  llvm::WriteGraph(os, Graph);
}

} // namespace simplecompiler

namespace simplecompiler {

/// This class generates a description for each AST node.
class DescriptionVisitor : public VisitorBase<DescriptionVisitor> {
public:
  /// Return a descriptive string for AR.
  String makeDescription(const AstRef &AR) {
    switch (AR.getKind()) {
#define DEFINE_AST_TRAITS(NAME)                                                \
  case AstKind::NAME:                                                          \
    return visit##NAME(static_cast<NAME *>(AR.get()));
      DEFINE_AST_TRAITS(Decl)
      DEFINE_AST_TRAITS(Expr)
      DEFINE_AST_TRAITS(Stmt)
#undef DEFINE_AST_TRAITS
    default:
      return "";
    }
  }

  /// VisitorBase boilderplate code.
  String visitDecl(Decl *D) { return VisitorBase::visitDecl<String>(D); }
  String visitExpr(Expr *E) { return VisitorBase::visitExpr<String>(E); }
  String visitStmt(Stmt *S) { return VisitorBase::visitStmt<String>(S); }

  String visitConstDecl(ConstDecl *CD) {
    std::ostringstream O;
    /// lambda to extract the numeric value of a Num or Char.
    auto MakeCV = [](Expr *E) {
      if (auto x = subclass_cast<Char>(E))
        return x->c;
      if (auto x = subclass_cast<Num>(E))
        return x->n;
      assert(false && "Unknown Expr class");
    };

    O << "const " << CStringFromBasicTypeKind(CD->type) << " " << CD->name
      << " = " << MakeCV(CD->value);
    return O.str();
  }

  String visitVarDecl(VarDecl *VD) {
    std::ostringstream O;
    O << CStringFromBasicTypeKind(VD->type) << " " << VD->name;
    if (VD->is_array) {
      O << "[" << VD->size << "]";
    }
    return O.str();
  }

  String visitFuncDef(FuncDef *FD) {
    std::ostringstream O;
    Print(O, CStringFromBasicTypeKind(FD->return_type), FD->name);
    return O.str();
  }

  String visitArgDecl(ArgDecl *A) {
    std::ostringstream O;
    Print(O, CStringFromBasicTypeKind(A->type), A->name);
    return O.str();
  }

  String visitBinOp(BinOp *BO) { return CStringFromOperatorKind(BO->op); }

  String visitUnaryOp(UnaryOp *UO) { return CStringFromUnaryopKind(UO->op); }

  String visitBoolOp(BoolOp *BO) { return ""; }

  String visitParenExpr(ParenExpr *PE) { return "()"; }

  String visitName(Name *N) { return N->id; }

  String visitNum(Num *N) {
    std::ostringstream O;
    O << N->n;
    return O.str();
  }

  String visitChar(Char *C) {
    std::ostringstream O;
    O << "'" << C->c << "'";
    return O.str();
  }

  String visitStr(Str *S) {
    std::ostringstream O;
    O << S->s;
    return O.str();
  }

  String visitCall(Call *C) { return C->func; }

  String visitRead(Read *) { return "scanf"; }

  String visitWrite(Write *) { return "printf"; }

  String visitAssign(Assign *) { return "="; }

  String visitSubscript(Subscript *) { return "[]"; }

  String visitExprStmt(ExprStmt *ES) { return visitExpr(ES->value); }

  String visitFor(For *) { return ""; }
  String visitIf(If *) { return ""; }
  String visitWhile(While *) { return ""; }
  String visitReturn(Return *) { return ""; }
};

} // namespace simplecompiler

namespace llvm {
using simplecompiler::AstGraph;
using simplecompiler::AstRef;

/// Specialized GraphTraits for AstGraph
template <> struct GraphTraits<AstGraph> {
  using NodeRef = AstRef *;
  using nodes_iterator = AstGraph::NodeIterator;
  using ChildIteratorType = AstGraph::ChildIteratorType;

  static nodes_iterator nodes_begin(const AstGraph &G) {
    return G.nodes_begin();
  }

  static nodes_iterator nodes_end(const AstGraph &G) { return G.nodes_end(); }

  static ChildIteratorType child_begin(NodeRef N) {
    return AstGraph::ChildBegin(*N);
  }

  static ChildIteratorType child_end(NodeRef N) {
    return AstGraph::ChildEnd(*N);
  }
};

/// Specialized DOTGraphTraits for AstGraph.
template <> struct DOTGraphTraits<AstGraph> : DefaultDOTGraphTraits {
  DOTGraphTraits(bool simple = false) : DefaultDOTGraphTraits(simple) {}

  static std::string getGraphName(const AstGraph &) {
    return "Abstract Syntax Tree";
  }

  std::string getNodeLabel(const void *NodeRef, const AstGraph &) {
    auto N = static_cast<AstRef *>(const_cast<void *>(NodeRef));
    return N->getClassName();
  }

  static std::string getNodeDescription(const void *NodeRef, const AstGraph &) {
    auto N = static_cast<AstRef *>(const_cast<void *>(NodeRef));
    simplecompiler::DescriptionVisitor DV;
    return DV.makeDescription(*N);
  }
};

} // namespace llvm
