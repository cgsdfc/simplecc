#ifndef NODE_H
#define NODE_H

#include "Grammar.h"
#include "simplecc/Lex/TokenInfo.h"

#include <cassert>
#include <iostream>
#include <vector>

namespace simplecc {
class Node {
  String FormatValue() const;
public:
  using ChildrenListT = std::vector<Node *>;

  Node(Symbol Ty, const String &Val, const Location &L)
      : Type(Ty), Value(Val), Children(), Loc(L) {}

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

  Node *FirstChild() { return getChild(0); }

  Node *LastChild() { return getChild(getNumChildren() - 1); }

  void Format(std::ostream &O) const;

  Node *getChild(unsigned Idx) const {
    assert(getNumChildren() >= 0 && Idx < getNumChildren());
    return Children[Idx];
  }

  unsigned getNumChildren() const { return Children.size(); }

  Symbol getType() const { return Type; }
  const char *getTypeName() const;
  const Location &getLocation() const { return Loc; }
  const String &getValue() const { return Value; }

private:
  Symbol Type;
  String Value;
  ChildrenListT Children;
  Location Loc;
};

inline std::ostream &operator<<(std::ostream &os, const Node &N) {
  N.Format(os);
  return os;
}
} // namespace simplecc

#endif
