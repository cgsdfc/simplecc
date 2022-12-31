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

#ifndef SIMPLECC_PARSE_NODE_H
#define SIMPLECC_PARSE_NODE_H
#include "simplecc/Lex/Location.h"
#include "simplecc/Parse/Grammar.h"
#include "simplecc/Support/Macros.h"
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <cassert>

namespace simplecc {
class Node {
public:
  using ChildrenListType = std::vector<Node *>;
  Node(Symbol Ty, std::string Val, Location L);
  ~Node();

  /// Iterator Interface to children Nodes.
  using iterator = ChildrenListType::iterator;
  using const_iterator = ChildrenListType::const_iterator;

  iterator begin() { return std::begin(Children); }
  iterator end() { return std::end(Children); }
  const_iterator begin() const { return std::begin(Children); }
  const_iterator end() const { return std::end(Children); }

  const std::vector<Node *> &getChildren() const { return Children; }
  std::vector<Node *> &getChildren() { return Children; }

  void AddChild(Node *child) { Children.push_back(child); }
  Node *getFirstChild() const { return getChild(0); }
  Node *getLastChild() const { return getChild(getNumChildren() - 1); }
  Node *getChild(unsigned Idx) const {
    assert(getNumChildren() >= 0 && Idx < getNumChildren());
    return Children[Idx];
  }
  size_t getNumChildren() const { return Children.size(); }

  Symbol getType() const { return Type; }
  const char *getTypeName() const;
  Location getLocation() const { return Loc; }
  const std::string &getValue() const { return Value; }
  void Format(std::ostream &O) const;
  void dump() const;

private:
  Symbol Type;
  std::string Value;
  ChildrenListType Children;
  Location Loc;
};

DEFINE_INLINE_OUTPUT_OPERATOR(Node)

} // namespace simplecc
#endif // SIMPLECC_PARSE_NODE_H