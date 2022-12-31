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

#ifndef SIMPLECC_SUPPORT_ITERATOR_RANGE_H
#define SIMPLECC_SUPPORT_ITERATOR_RANGE_H

#include <iterator>
#include <utility>

namespace simplecc {

/// A range adaptor for a pair of iterators.
///
/// This just wraps two iterators into a range-compatible interface. Nothing
/// fancy at all.
template <typename IteratorT> class iterator_range {
  IteratorT begin_iterator, end_iterator;

public:
  template <typename Container>
  iterator_range(Container &&c)
      : begin_iterator(c.begin()), end_iterator(c.end()) {}
  iterator_range(IteratorT begin_iterator, IteratorT end_iterator)
      : begin_iterator(std::move(begin_iterator)),
        end_iterator(std::move(end_iterator)) {}

  IteratorT begin() const { return begin_iterator; }
  IteratorT end() const { return end_iterator; }
};

/// Convenience function for iterating over sub-ranges.
///
/// This provides a bit of syntactic sugar to make using sub-ranges
/// in for loops a bit easier. Analogous to std::make_pair().
template <class T> iterator_range<T> make_range(T x, T y) {
  return iterator_range<T>(std::move(x), std::move(y));
}

template <typename T> iterator_range<T> make_range(std::pair<T, T> p) {
  return iterator_range<T>(std::move(p.first), std::move(p.second));
}
} // namespace simplecc

#endif