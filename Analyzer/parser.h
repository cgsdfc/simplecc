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

  void Format(std::ostream &os) const {
    const char *type_str = GetSymName(type);
    os << "Node(";
    os << "type=" << type_str << ", ";
    os << "value=" << FormatValue() << ", ";
    os << "context=" << location << ", ";
    os << "children=";
    if (children.empty()) {
      os << "None)";
    } else {
      os << "[";
      for (int i = 0; i < children.size(); i++) {
        children[i]->Format(os);
        if (i != children.size() - 1)
          os << ", ";
      }
      os << "])";
    }
  }

};

inline std::ostream &operator<<(std::ostream &os, const Node &node) {
  node.Format(os);
  return os;
}

Node *ParseTokens(const TokenBuffer &tokens);

#endif
