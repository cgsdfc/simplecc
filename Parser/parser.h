#ifndef PARSER_H
#define PARSER_H

#include "tokenize.h"

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

  void Format(std::FILE *file) {
    const char *type_str = GetSymName(type);

    fprintf(file, "(%s", type_str);
    if (value.size()) {
      fprintf(file, ", %s", value.c_str());
    }
    fprintf(file, ", (\n");

    if (children.size()) {
      for (auto child: children) {
        child->Format(file);
        fprintf(file, ", ");
      }
    }
    fprintf(file, ")");
  }

};

#endif
