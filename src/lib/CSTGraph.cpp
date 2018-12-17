#include "simplecompiler/CSTGraph.h"
#include "simplecompiler/Node.h"
#include "simplecompiler/Print.h"

#include <llvm/ADT/GraphTraits.h>
#include <llvm/ADT/iterator.h>
#include <llvm/Support/GraphWriter.h>
#include <llvm/Support/raw_ostream.h>

#include <cassert>
#include <iostream>
#include <iterator>
#include <stack>
#include <string>

namespace simplecompiler {
using CSTGraphTy = const Node *;

class NodeIteratorImpl {
  /// Keep track of Node to visit
  std::stack<Node *> Stack;

public:
  /// begin
  NodeIteratorImpl(Node *R) : Stack() {
    assert(R && "Cannot iterate a null Node");
    Stack.push(R);
  }

  /// end
  NodeIteratorImpl() : Stack() {}

  /// Return the next Node or nullptr.
  Node *getNext() {
    if (Stack.empty())
      return nullptr;
    Node *Tos = Stack.top();
    Stack.pop();
    for (Node *Child : Tos->getChildren()) {
      Stack.push(Child);
    }
    return Tos;
  }
};

/// A readonly iterator of the CST's Node
class NodeIterator
    : public llvm::iterator_facade_base<NodeIterator, std::forward_iterator_tag,
                                        Node> {
  using Base =
      llvm::iterator_facade_base<NodeIterator, std::forward_iterator_tag, Node>;
  using Self = NodeIterator;

public:
  using pointer = Node *;
  using const_pointer = const Node *;
  using reference = Node &;
  using const_reference = const Node &;

  explicit NodeIterator(Node *R) : Impl(R) { operator++(); }

  NodeIterator() : Impl(), NodeRef() {}

  bool operator==(const Self &O) const { return NodeRef == O.NodeRef; }

  const_pointer operator*() const {
    assert(NodeRef);
    return NodeRef;
  }

  pointer operator*() {
    return const_cast<pointer>(const_cast<const Self *>(this)->operator*());
  }

  Self &operator++() {
    NodeRef = Impl.getNext();
    return *this;
  }

  /// Return begin iterator
  static Self begin(pointer NodeRef) { return Self(NodeRef); }

  // Return end iterator
  static Self end() { return Self(); }

  static llvm::iterator_range<Self> getIter(pointer NodeRef) {
    return llvm::make_range(begin(NodeRef), end());
  }

private:
  NodeIteratorImpl Impl;
  pointer NodeRef;
};

} // namespace simplecompiler

namespace llvm {
using simplecompiler::Node;
using CSTGraphTy = Node *;

/// Specialized GraphTraits
template <> struct GraphTraits<CSTGraphTy> {
  using NodeRef = CSTGraphTy;
  using nodes_iterator = simplecompiler::NodeIterator;
  using ChildIteratorType = Node::const_iterator;

  static nodes_iterator nodes_begin(const CSTGraphTy &G) {
    return simplecompiler::NodeIterator::begin(G);
  }

  static nodes_iterator nodes_end(const CSTGraphTy &) {
    return simplecompiler::NodeIterator::end();
  }

  static ChildIteratorType child_begin(NodeRef N) { return N->begin(); }

  static ChildIteratorType child_end(NodeRef N) { return N->end(); }
};

template <> struct DOTGraphTraits<CSTGraphTy> : DefaultDOTGraphTraits {
  DOTGraphTraits(bool simple = false) : DefaultDOTGraphTraits(simple) {}

  static std::string getGraphName(const CSTGraphTy &) {
    return "Concrete Syntax Tree";
  }

  std::string getNodeLabel(const void *NodeRef, const CSTGraphTy &) {
    auto N = static_cast<Node *>(const_cast<void *>(NodeRef));
    return N->getTypeName();
  }

  static std::string getNodeDescription(const void *NodeRef,
                                        const CSTGraphTy &) {
    auto N = static_cast<Node *>(const_cast<void *>(NodeRef));
    return N->getValue();
  }
};

} // namespace llvm

namespace simplecompiler {
void PrintAllNodes(Node *Root) {
  for (const Node *N : NodeIterator::getIter(Root)) {
    Print(std::cout, N->getTypeName(), N->getValue(), N->getLocation());
  }
}

void WriteCSTGraph(Node *Root, llvm::raw_ostream &os) {
  llvm::WriteGraph(os, Root);
}

} // namespace simplecompiler
