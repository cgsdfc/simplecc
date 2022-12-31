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

#include "simplecc/Visualize/ASTGraph.h"
#include "simplecc/Support/Print.h"
#include "simplecc/Visualize/ChildrenCollector.h"
#include "simplecc/Visualize/DescriptionVisitor.h"
#include "simplecc/Visualize/Visualize.h"

#include <llvm/ADT/GraphTraits.h>
#include <llvm/Support/GraphWriter.h>

using namespace simplecc;

ASTNode *ASTGraph::getNodeOrCreate(AST *Ptr) {
  auto iter = Nodes.find(Ptr);
  if (iter != Nodes.end())
    return iter->second.get();
  auto Result = Nodes.emplace(Ptr, llvm::make_unique<ASTNode>(Ptr, this));
  assert(Result.second && "Emplace must succeed");
  return Result.first->second.get();
}

const std::vector<ASTNode *> &ASTGraph::getEdgeOrCreate(const ASTNode &R) {
  auto iter = Edges.find(R.get());
  if (iter != Edges.end())
    return iter->second;

  std::vector<ASTNode *> E;
  ChildrenCollector C(E, this);
  C.Collect(R);

  auto Result = Edges.emplace(R.get(), std::move(E));
  assert(Result.second && "Emplace must succeed");
  return Result.first->second;
}

namespace llvm {
using simplecc::ASTGraph;
using simplecc::ASTNode;

/// Specialized GraphTraits for ASTGraph
template <> struct GraphTraits<ASTGraph> {
  using NodeRef = ASTNode *;
  using nodes_iterator = ASTGraph::NodeIterator;
  using ChildIteratorType = ASTGraph::ChildIteratorType;

  static nodes_iterator nodes_begin(const ASTGraph &G) {
    return G.nodes_begin();
  }

  static nodes_iterator nodes_end(const ASTGraph &G) { return G.nodes_end(); }

  static ChildIteratorType child_begin(NodeRef N) {
    return (*N).getParent()->child_begin(*N);
  }

  static ChildIteratorType child_end(NodeRef N) {
    return (*N).getParent()->child_end(*N);
  }
};

/// Specialized DOTGraphTraits for ASTGraph.
template <> struct DOTGraphTraits<ASTGraph> : DefaultDOTGraphTraits {
  explicit DOTGraphTraits(bool simple = false)
      : DefaultDOTGraphTraits(simple) {}

  static std::string getGraphName(const ASTGraph &G) {
    return G.getProgram()->getFilename();
  }

  std::string getNodeLabel(const void *NodeRef, const ASTGraph &) {
    auto N = static_cast<ASTNode *>(const_cast<void *>(NodeRef));
    return N->getClassName();
  }

  static std::string getNodeDescription(const void *NodeRef, const ASTGraph &) {
    auto N = static_cast<ASTNode *>(const_cast<void *>(NodeRef));
    simplecc::DescriptionVisitor DV;
    return DV.makeDescription(*N);
  }
};
} // namespace llvm

namespace simplecc {
/// Write an ASTGraph to dot format.
void WriteASTGraph(ProgramAST *P, llvm::raw_ostream &O) {
  ASTGraph Graph(P);
  llvm::WriteGraph(O, Graph);
}

/// Print all ast nodes from a root.
void PrintAllASTNodes(ProgramAST *P, std::ostream &O) {
  ASTGraph Graph(P);
  for (auto AR : Graph.nodes()) {
    Print(O, AR->getClassName(), AR->getLocation());
  }
}
} // namespace simplecc