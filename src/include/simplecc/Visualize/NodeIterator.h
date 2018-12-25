#ifndef SIMPLECC_VISUALIZE_NODEITERATOR_H
#define SIMPLECC_VISUALIZE_NODEITERATOR_H
#include <llvm/ADT/iterator.h>

#include <cassert>
#include <stack>

namespace simplecc {
class Node;

class NodeIteratorImpl {
  /// Keep track of Node to visit
  std::stack<Node *> TheStack;

public:
  /// begin
  NodeIteratorImpl(Node *R) : TheStack() {
    assert(R && "Cannot iterate a null Node");
    TheStack.push(R);
  }
  /// end
  NodeIteratorImpl() : TheStack() {}
  Node *getNext();
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

} // namespace simplecc

#endif // SIMPLECC_VISUALIZE_NODEITERATOR_H
