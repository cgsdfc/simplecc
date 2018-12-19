#ifndef ASTGRAPH_H
#define ASTGRAPH_H
#include "simplecc/AstIterator.h"
#include "simplecc/AstRef.h"
#include "simplecc/AST.h"

#include <llvm/ADT/STLExtras.h>

#include <iostream>
#include <map>
#include <vector>
#include <memory> // for make_unique
#include <cassert>


namespace llvm {
class raw_ostream;
}

namespace simplecc {
class Program;
class AstRef;
class AST;

// A class that keeps all the edges of an AstRef
class AstGraph {
public:
  AstGraph(Program *P) : TheProgram(P), Edges(), Nodes() {}

  using NodeIterator = AstIterator;
  using ChildIteratorType = std::vector<AstRef *>::const_iterator;

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

  llvm::iterator_range<ChildIteratorType> children(const AstRef &R) {
    return llvm::make_range(child_begin(R), child_end(R));
  }

  /// Lazily create edges for a Node.
  const std::vector<AstRef *> &getEdgeOrCreate(const AstRef &R);

  /// Lazily create a node, namely an AstRef
  template<typename AstT> AstRef *getNodeOrCreate(AstT *Ptr);

private:
  /// Root of AST.
  Program *TheProgram;
  /// Lazily created edges for each node.
  std::map<AST *, std::vector<AstRef *>> Edges;
  /// Lazily created nodes.
  std::map<AST *, std::unique_ptr<AstRef>> Nodes;
};

template<typename AstT>
AstRef *AstGraph::getNodeOrCreate(AstT *Ptr) {
  auto iter = Nodes.find(Ptr);
  if (iter != Nodes.end())
    return iter->second.get();
  auto Result = Nodes.emplace(Ptr, llvm::make_unique<AstRef>(Ptr, this));
  assert(Result.second && "Emplace must succeed");
  return Result.first->second.get();
}

void PrintAllAstNodes(Program *P, std::ostream &os);
void WriteASTGraph(Program *P, llvm::raw_ostream &O);
} // namespace simplecc

#endif
