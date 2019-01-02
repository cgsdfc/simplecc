#ifndef SIMPLECC_PARSE_NODE_H
#define SIMPLECC_PARSE_NODE_H
#include "simplecc/Lex/Location.h"
#include "simplecc/Parse/Grammar.h"
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

inline std::ostream &operator<<(std::ostream &os, const Node &N) {
  N.Format(os);
  return os;
}
} // namespace simplecc

#endif