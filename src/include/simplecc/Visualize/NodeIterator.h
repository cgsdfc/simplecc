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