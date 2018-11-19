#ifndef PARSER_H
#define PARSER_H

#include "tokenize.h"

class Node {
public:
  Symbol type;
  String value;
  std::vector<Node*> children;
  Location location;

  Node(Symbol type, const String &value, Location location):
    type(type), value(value), children(), location(location) {}

  ~Node();

  void AddChild(Node *child) {
    children.push_back(child);
  }

  String FormatValue() const;

  Node *FirstChild() {
    return children[0];
  }

  Node *LastChild() {
    return *(children.end() - 1);
  }

  void Format(std::ostream &os) const;

};

inline std::ostream &operator<<(std::ostream &os, const Node &node) {
  node.Format(os);
  return os;
}

Node *ParseTokens(const TokenBuffer &tokens);

#endif
