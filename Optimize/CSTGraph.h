#ifndef CST_GRAPH_H
#define CST_GRAPH_H

#include "Node.h"
#include "Print.h"

#include <llvm/ADT/GraphTraits.h>
#include <llvm/ADT/iterator.h>
#include <llvm/Support/raw_ostream.h>

#include <stack>
#include <cassert>

namespace simplecompiler {
class NodeIteratorImpl {
  /// Keep track of Node to visit
  std::stack<Node*> Stack;
public:
  /// begin
  NodeIteratorImpl(Node *R): Stack() {
    assert(R && "Cannot iterate a null Node");
    Stack.push(R);
  }

  /// end
  NodeIteratorImpl(): Stack() {}

  /// Return the next Node or nullptr.
  Node *GetNext() {
    if (Stack.empty()) return nullptr;
    Node *Tos = Stack.top();
    Stack.pop();
    for (Node *Child : Tos->children) {
      Stack.push(Child);
    }
    return Tos;
  }

};

/// A readonly iterator of the CST's Node
class NodeIterator:
  public llvm::iterator_facade_base<NodeIterator, std::forward_iterator_tag, Node> {
    using Base = llvm::iterator_facade_base<NodeIterator, std::forward_iterator_tag, Node>;
    using Self = NodeIterator;

public:
    using pointer = Node *;
    using const_pointer = const Node *;
    using reference = Node &;
    using const_reference = const Node &;

  explicit NodeIterator(Node *R): Impl(R) {
    operator++();
  }

  NodeIterator(): Impl(), NodeRef() {}

  bool operator==(const Self &O) const { return NodeRef == O.NodeRef; }

  const_pointer operator*() const {
    assert(NodeRef);
    return NodeRef;
  }

  pointer operator*() {
    return const_cast<pointer>(
        const_cast<const Self*>(this)->operator*());
  }

  Self &operator++() {
    NodeRef = Impl.GetNext();
    return *this;
  }

  /// Return begin iterator
  static Self begin(pointer NodeRef) {
    return Self(NodeRef);
  }

  // Return end iterator
  static Self end() {
    return Self();
  }

  static llvm::iterator_range<Self> GetIter(pointer NodeRef) {
    return llvm::make_range(begin(NodeRef), end());
  }

private:
  NodeIteratorImpl Impl;
  pointer NodeRef;

};

void PrintAllNodes(Node *Root);
void WriteCSTGraph(Node *Root, llvm::raw_ostream &os);

using NodeChildIterator = decltype(Node::children)::iterator;

inline NodeChildIterator NodeChildBegin(Node *N) {
  return std::begin(N->children);
}

inline NodeChildIterator NodeChildEnd(Node *N) {
  return std::end(N->children);
}

} // namespace simplecompiler

namespace llvm {
/// Specialized GraphTraits
template <> struct GraphTraits<simplecompiler::Node*> {
  using NodeRef = simplecompiler::Node *;
  using nodes_iterator = simplecompiler::NodeIterator;
  using ChildIteratorType = simplecompiler::NodeChildIterator;

  static nodes_iterator nodes_begin(simplecompiler::Node *N) {
    return simplecompiler::NodeIterator::begin(N);
  }

  static nodes_iterator nodes_end(simplecompiler::Node *) {
    return simplecompiler::NodeIterator::end();
  }

  static ChildIteratorType child_begin(NodeRef N) {
    return simplecompiler::NodeChildBegin(N);
  }

  static ChildIteratorType child_end(NodeRef N) {
    return simplecompiler::NodeChildEnd(N);
  }
};
} // namespace llvm

#endif
