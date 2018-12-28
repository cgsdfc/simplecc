#ifndef SIMPLECC_VISUALIZE_ASTGRAPH_H
#define SIMPLECC_VISUALIZE_ASTGRAPH_H
#include "simplecc/Parse/AST.h"
#include "simplecc/Visualize/AstIterator.h"
#include "simplecc/Visualize/AstRef.h"

#include <llvm/ADT/STLExtras.h>

#include <cassert>
#include <map>
#include <memory> // for make_unique
#include <vector>

namespace simplecc {
class Program;
class AstRef;
class AST;

// A class that keeps all the edges of an AstRef
class AstGraph {
public:
  explicit AstGraph(Program *P) : TheProgram(P), Edges(), Nodes() {}

  using NodeIterator = AstIterator;
  using ChildIteratorType = AstIterator::ChildIteratorType;

  /// Iterate over all nodes.
  NodeIterator nodes_begin() const {
    return NodeIterator(TheProgram, const_cast<AstGraph *>(this));
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

  /// Lazily create edges for a Node.
  const std::vector<AstRef *> &getEdgeOrCreate(const AstRef &R);

  /// Lazily create a node, namely an AstRef.
  AstRef *getNodeOrCreate(AST *Ptr);

private:
  /// Root of AST.
  Program *TheProgram;
  /// Lazily created edges for each node.
  std::map<AST *, std::vector<AstRef *>> Edges;
  /// Lazily created nodes.
  std::map<AST *, std::unique_ptr<AstRef>> Nodes;
};

} // namespace simplecc

#endif
