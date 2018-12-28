#ifndef SIMPLECC_VISUALIZE_TREELIKEITERATOR_H
#define SIMPLECC_VISUALIZE_TREELIKEITERATOR_H

#include <llvm/ADT/iterator_range.h>
#include <llvm/ADT/iterator.h>
#include <stack>

namespace simplecc {

/// This is a CRTP base class can be used as an iterator to any
/// tree-like object, provided that the Derived implement a getEdges()
/// method, which fetches edges for a given node.
template<typename Derived, typename ValueTy, typename EdgeIter>
class TreeLikeIterator : public llvm::iterator_facade_base<TreeLikeIterator<Derived, ValueTy, EdgeIter>,
                                                           std::forward_iterator_tag, ValueTy> {
public:
  using value_type = ValueTy;
  using ChildIteratorType = EdgeIter;

  TreeLikeIterator(value_type Root) {
    assert(Root && "cannot iterate null node");
    TheStack.push(Root);
    Val = getNext();
  }
  TreeLikeIterator() : TheStack(), Val() {}

  bool operator==(const TreeLikeIterator &RHS) const {
    return Val == RHS.Val;
  }
  value_type operator*() const { return Val; }
  TreeLikeIterator &operator++() {
    Val = getNext();
    return *this;
  }

protected:
  using EdgeRange = llvm::iterator_range<EdgeIter>;
  /// The default makes an empty range.
  EdgeRange getEdges(value_type) {
    return llvm::make_range(EdgeIter(), EdgeIter());
  }

private:
  std::stack<value_type> TheStack;
  value_type Val;

  value_type getNext() {
    if (TheStack.empty())
      return value_type();
    value_type Tos = TheStack.top();
    TheStack.pop();
    for (value_type Child : static_cast<Derived *>(this)->getEdges(Tos)) {
      TheStack.push(Child);
    }
    return Tos;
  }
};
}

#endif //SIMPLECC_VISUALIZE_TREELIKEITERATOR_H
