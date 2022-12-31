// MIT License
// 
// Copyright (c) 2018 Cong Feng.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef SIMPLECC_VISUALIZE_ASTGRAPH_H
#define SIMPLECC_VISUALIZE_ASTGRAPH_H
#include "simplecc/AST/AST.h"
#include "simplecc/Visualize/ASTNode.h"
#include "simplecc/Visualize/TreeLikeIterator.h"
#include <llvm/ADT/STLExtras.h>
#include <map>
#include <memory>
#include <vector>
#include <cassert>

namespace simplecc {
/// A class that keeps all the edges of an ASTNode.
class ASTGraph {
public:
  explicit ASTGraph(ProgramAST *P) : TheProgram(P), Edges(), Nodes() {}

  using EdgeType = std::vector<ASTNode *>;
  using ChildIteratorType = EdgeType::const_iterator;

  /// Iterator to all nodes of a graph.
  class ASTIterator
      : public TreeLikeIterator<ASTIterator, ASTNode *, ChildIteratorType> {
  public:
    ASTIterator(ProgramAST *P, ASTGraph *G) : TreeLikeIterator(), Parent(G) {
      Initialize(Parent->getNodeOrCreate(P));
    }
    ASTIterator() = default;

    EdgeRange getEdges(value_type N) {
      const auto &LazyEdges = Parent->getEdgeOrCreate(*N);
      return llvm::make_range(LazyEdges.begin(), LazyEdges.end());
    }

  private:
    ASTGraph *Parent = nullptr;
  };
  using NodeIterator = ASTIterator;

  /// Iterate over all nodes.
  NodeIterator nodes_begin() const {
    return NodeIterator(TheProgram, const_cast<ASTGraph *>(this));
  }
  NodeIterator nodes_end() const { return NodeIterator(); }

  llvm::iterator_range<NodeIterator> nodes() {
    return llvm::make_range(nodes_begin(), nodes_end());
  }

  /// Iterate all children of one node.
  ChildIteratorType child_begin(const ASTNode &R) {
    return std::begin(getEdgeOrCreate(R));
  }
  ChildIteratorType child_end(const ASTNode &R) {
    return std::end(getEdgeOrCreate(R));
  }

  /// Lazily create edges for a Node.
  const std::vector<ASTNode *> &getEdgeOrCreate(const ASTNode &R);

  /// Lazily create an ASTNode.
  ASTNode *getNodeOrCreate(AST *Ptr);

  const ProgramAST *getProgram() const { return TheProgram; }

private:
  /// Root of AST.
  ProgramAST *TheProgram;
  /// Lazily created edges for each node.
  std::map<AST *, EdgeType> Edges;
  /// Lazily created nodes.
  std::map<AST *, std::unique_ptr<ASTNode>> Nodes;
};
} // namespace simplecc

#endif