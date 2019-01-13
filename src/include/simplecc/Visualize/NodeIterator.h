#ifndef SIMPLECC_VISUALIZE_NODEITERATOR_H
#define SIMPLECC_VISUALIZE_NODEITERATOR_H
#include "simplecc/Parse/Node.h"
#include "simplecc/Visualize/TreeLikeIterator.h"
#include <cassert>
#include <stack>

namespace simplecc {
/// This class implements an iterator to the CST graph.
class NodeIterator
    : public TreeLikeIterator<NodeIterator, Node *, Node::const_iterator> {
public:
  NodeIterator(value_type Root) : TreeLikeIterator(Root) {}
  NodeIterator() = default;

  EdgeRange getEdges(value_type N) {
    return llvm::make_range(N->begin(), N->end());
  }
};

inline llvm::iterator_range<NodeIterator> makeNodeRange(Node *Root) {
  return llvm::make_range(NodeIterator(Root), NodeIterator());
}

} // namespace simplecc

#endif // SIMPLECC_VISUALIZE_NODEITERATOR_H