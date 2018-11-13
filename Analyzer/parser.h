#ifndef PARSER_H
#define PARSER_H

#include "tokenize.h"

inline String Quote(const String &string) {
  return '\'' + string + '\'';
}

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

  String FormatValue() const {
    return type != Symbol::ENDMARKER && value.empty() ? "None" : Quote(value);
  }

  Node *FirstChild() {
    return children[0];
  }

  Node *LastChild() {
    return *(children.end() - 1);
  }

  void Format(std::ostream &os) {
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

Node *ParseTokens(const TokenBuffer &tokens);

template <typename... Arg>
void error(Arg&&... args);

template <typename First, typename... Rest>
inline void error(First&& first, Rest&&... rest) {
  std::cerr << first << " ";
  error(rest...);
}

template<>
inline void error() {
  std::cerr << "\n";
}

template <typename... Arg>
inline void Error(const Location &loc, Arg&&... args) {
  fprintf(stderr, "Error in line %d column %d: ", loc.lineno, loc.col_offset);
  error(args...);
}


#endif
