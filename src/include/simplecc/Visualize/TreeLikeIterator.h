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

#ifndef SIMPLECC_VISUALIZE_TREELIKEITERATOR_H
#define SIMPLECC_VISUALIZE_TREELIKEITERATOR_H
#include <llvm/ADT/iterator.h>
#include <llvm/ADT/iterator_range.h>
#include <stack>
#include <cassert>

namespace simplecc {

/// This is a CRTP base class can be used as an iterator to any
/// tree-like object, provided that the Derived implement a getEdges()
/// method, which fetches edges for a given node.
template <typename Derived, typename ValueTy, typename ChildIter>
class TreeLikeIterator : public llvm::iterator_facade_base<
                             TreeLikeIterator<Derived, ValueTy, ChildIter>,
                             std::forward_iterator_tag, ValueTy> {
public:
  using value_type = ValueTy;
  using ChildIteratorType = ChildIter;

  /// This ctor is the first-phrase initialization.
  explicit TreeLikeIterator(value_type Root) { Initialize(Root); }
  TreeLikeIterator() : TheStack(), Val() {}

  /// Requirements for llvm::iterator_facade_base.
  bool operator==(const TreeLikeIterator &RHS) const { return Val == RHS.Val; }
  value_type operator*() const { return Val; }
  TreeLikeIterator &operator++() {
    Val = getNext();
    return *this;
  }

protected:
  /// The type returned by getEdges().
  using EdgeRange = llvm::iterator_range<ChildIter>;
  /// The default makes an empty range.
  EdgeRange getEdges(value_type) {
    return llvm::make_range(ChildIter(), ChildIter());
  }

  // Second-phrase initialization. Derived can use it if initializing via base class
  // construction is awkward.
  void Initialize(value_type Root) {
    assert(Root && "cannot iterate null node");
    assert(TheStack.empty());
    TheStack.push(Root);
    Val = getNext();
  }

private:
  /// This backs up the DFS search.
  std::stack<value_type> TheStack;
  /// This is the current node.
  value_type Val;

  /// Return the next available node from the graph.
  /// If none is available, return nullptr.
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
} // namespace simplecc

#endif // SIMPLECC_VISUALIZE_TREELIKEITERATOR_H