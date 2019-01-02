#ifndef SIMPLECC_PARSE_NODE_H
#define SIMPLECC_PARSE_NODE_H
#include "simplecc/Lex/Location.h"
#include "simplecc/Parse/Grammar.h"

#include <string>
#include <cassert>
#include <iostream>
#include <vector>
#include <utility>

namespace simplecc {
class Node {
public:
  using ChildrenListT = std::vector<Node *>;

  Node(Symbol Ty, std::string Val, Location L)
      : Type(Ty), Value(std::move(Val)), Children(), Loc(L) {}

  ~Node();

  /// Iterator Interface to children Nodes.
  using iterator = ChildrenListT::iterator;
  using const_iterator = ChildrenListT::const_iterator;

  iterator begin() { return std::begin(Children); }
  iterator end() { return std::end(Children); }

  const_iterator begin() const { return std::begin(Children); }
  const_iterator end() const { return std::end(Children); }

  const std::vector<Node *> &getChildren() const { return Children; }
  std::vector<Node *> &getChildren() { return Children; }

  void AddChild(Node *child) { Children.push_back(child); }

  Node *FirstChild() const { return getChild(0); }

  Node *LastChild() const { return getChild(getNumChildren() - 1); }

  Node *getChild(unsigned Idx) const {
    assert(getNumChildren() >= 0 && Idx < getNumChildren());
    return Children[Idx];
  }

  unsigned getNumChildren() const { return Children.size(); }

  Symbol getType() const { return Type; }

  const char *getTypeName() const;
  const Location &getLocation() const { return Loc; }
  const std::string &getValue() const { return Value; }
  void Format(std::ostream &O) const;

private:
  Symbol Type;
  std::string Value;
  ChildrenListT Children;
  Location Loc;
};

inline std::ostream &operator<<(std::ostream &os, const Node &N) {
  N.Format(os);
  return os;
}
} // namespace simplecc

#endif
