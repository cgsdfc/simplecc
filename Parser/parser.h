#ifndef PARSER_H
#define PARSER_H

#include "tokenize.h"

inline String Quote(const String &string) {
  return '\'' + string + '\'';
}

class Node {
public:
  int type;
  String value;
  std::vector<Node*> children;
  Location location;

  Node(int type, const String &value, Location location):
    type(type), value(value), children(), location(location) {}

  void AddChild(Node *child) {
    children.push_back(child);
  }

  String FormatValue() const {
    return type != ENDMARKER && value.empty() ? "None" : Quote(value);
  }

  void Format(std::ostream &os) {
    const char *type_str = GetSymName(type);
    os << "Node(";
    os << "type=" << type << ", ";
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

#endif
