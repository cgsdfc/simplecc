#ifndef NODE_H
#define NODE_H

#include "simplecc/Grammar.h"
#include "simplecc/TokenInfo.h"

#include <cassert>
#include <iostream>
#include <vector>

namespace simplecc {
class Node {
  Symbol type;
  String value;
  std::vector<Node *> children;
  Location location;

  String FormatValue() const;

public:
  Node(Symbol type, const String &value, const Location &location)
      : type(type), value(value), children(), location(location) {}

  ~Node();

  using iterator = decltype(children)::iterator;
  using const_iterator = decltype(children)::const_iterator;

  iterator begin() { return std::begin(children); }
  iterator end() { return std::end(children); }

  const_iterator begin() const { return std::begin(children); }
  const_iterator end() const { return std::end(children); }

  const std::vector<Node *> getChildren() const { return children; }

  void AddChild(Node *child) { children.push_back(child); }

  Node *FirstChild() { return children[0]; }

  Node *LastChild() { return *(children.end() - 1); }

  void Format(std::ostream &os) const;

  Node *getChild(unsigned pos) const {
    assert(0 <= pos && pos < getNumChildren());
    return children[pos];
  }

  unsigned getNumChildren() const { return children.size(); }

  Symbol GetType() const { return type; }
  Symbol getType() const { return type; }
  const char *getTypeName() const;
  const Location &GetLocation() const { return location; }
  const Location &getLocation() const { return location; }
  const String &GetValue() const { return value; }
  const String &getValue() const { return value; }
};

inline std::ostream &operator<<(std::ostream &os, const Node &node) {
  node.Format(os);
  return os;
}
} // namespace simplecc

#endif
