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

#include "simplecc/Parse/Node.h"
#include "simplecc/Support/Print.h"
#include "simplecc/Visualize/NodeIterator.h"
#include "simplecc/Visualize/Visualize.h"

#include <llvm/ADT/GraphTraits.h>
#include <llvm/Support/GraphWriter.h>

namespace llvm {
using simplecc::NodeIterator;
using CSTGraphTy = NodeIterator::value_type;

/// Specialized GraphTraits
template <> struct GraphTraits<CSTGraphTy> {
  using NodeRef = CSTGraphTy;
  using nodes_iterator = NodeIterator;
  using ChildIteratorType = NodeIterator::ChildIteratorType;

  static nodes_iterator nodes_begin(const CSTGraphTy &G) {
    return simplecc::NodeIterator(G);
  }

  static nodes_iterator nodes_end(const CSTGraphTy &) {
    return simplecc::NodeIterator();
  }

  static ChildIteratorType child_begin(NodeRef N) { return N->begin(); }
  static ChildIteratorType child_end(NodeRef N) { return N->end(); }
};

template <> struct DOTGraphTraits<CSTGraphTy> : DefaultDOTGraphTraits {
  explicit DOTGraphTraits(bool simple = false)
      : DefaultDOTGraphTraits(simple) {}

  static std::string getGraphName(const CSTGraphTy &) {
    return "Concrete Syntax Tree";
  }

  std::string getNodeLabel(const void *NodeRef, const CSTGraphTy &) {
    auto N = static_cast<CSTGraphTy>(const_cast<void *>(NodeRef));
    return N->getTypeName();
  }

  static std::string getNodeDescription(const void *NodeRef,
                                        const CSTGraphTy &) {
    auto N = static_cast<CSTGraphTy>(const_cast<void *>(NodeRef));
    return N->getValue();
  }
};
} // namespace llvm

namespace simplecc {

void PrintAllNodes(Node *Root) {
  for (const Node *N : makeNodeRange(Root)) {
    Print(std::cout, N->getTypeName(), N->getValue(), N->getLocation());
  }
}

void WriteCSTGraph(Node *Root, llvm::raw_ostream &O) {
  llvm::WriteGraph(O, Root);
}
} // namespace simplecc
