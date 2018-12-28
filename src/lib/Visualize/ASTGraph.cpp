#include "simplecc/Visualize/ASTGraph.h"
#include "simplecc/Support/Print.h"
#include "simplecc/Visualize/ChildrenCollector.h"
#include "simplecc/Visualize/DescriptionVisitor.h"
#include "simplecc/Visualize/Visualize.h"

#include <llvm/ADT/GraphTraits.h>
#include <llvm/Support/GraphWriter.h>

using namespace simplecc;

AstRef *AstGraph::getNodeOrCreate(AST *Ptr) {
  auto iter = Nodes.find(Ptr);
  if (iter != Nodes.end())
    return iter->second.get();
  auto Result = Nodes.emplace(Ptr, llvm::make_unique<AstRef>(Ptr, this));
  assert(Result.second && "Emplace must succeed");
  return Result.first->second.get();
}

const std::vector<AstRef *> &AstGraph::getEdgeOrCreate(const AstRef &R) {
  auto iter = Edges.find(R.get());
  if (iter != Edges.end())
    return iter->second;

  std::vector<AstRef *> E;
  ChildrenCollector C(E, this);
  C.Collect(R);

  auto Result = Edges.emplace(R.get(), std::move(E));
  assert(Result.second && "Emplace must succeed");
  return Result.first->second;
}

namespace llvm {
using simplecc::AstGraph;
using simplecc::AstRef;

/// Specialized GraphTraits for AstGraph
template<> struct GraphTraits<AstGraph> {
  using NodeRef = AstRef *;
  using nodes_iterator = AstGraph::NodeIterator;
  using ChildIteratorType = AstGraph::ChildIteratorType;

  static nodes_iterator nodes_begin(const AstGraph &G) {
    return G.nodes_begin();
  }

  static nodes_iterator nodes_end(const AstGraph &G) { return G.nodes_end(); }

  static ChildIteratorType child_begin(NodeRef N) {
    return (*N).getParent()->child_begin(*N);
  }

  static ChildIteratorType child_end(NodeRef N) {
    return (*N).getParent()->child_end(*N);
  }
};

/// Specialized DOTGraphTraits for AstGraph.
template<> struct DOTGraphTraits<AstGraph> : DefaultDOTGraphTraits {
  explicit DOTGraphTraits(bool simple = false)
      : DefaultDOTGraphTraits(simple) {}

  static std::string getGraphName(const AstGraph &G) {
    return G.getProgram()->getFilename();
  }

  std::string getNodeLabel(const void *NodeRef, const AstGraph &) {
    auto N = static_cast<AstRef *>(const_cast<void *>(NodeRef));
    return N->getClassName();
  }

  static std::string getNodeDescription(const void *NodeRef, const AstGraph &) {
    auto N = static_cast<AstRef *>(const_cast<void *>(NodeRef));
    simplecc::DescriptionVisitor DV;
    return DV.makeDescription(*N);
  }
};
} // namespace llvm

namespace simplecc {
/// Write an AstGraph to dot format.
void WriteASTGraph(Program *P, llvm::raw_ostream &O) {
  AstGraph Graph(P);
  llvm::WriteGraph(O, Graph);
}
} // namespace simplecc