#include "Node.h"
#include "error.h"
#include <algorithm>

using namespace simplecompiler;

Node::~Node() {
  std::for_each(children.begin(), children.end(), [](Node *N) { delete N; });
}

String Node::FormatValue() const {
  return type != Symbol::ENDMARKER && value.empty() ? "None" : Quote(value);
}

void Node::Format(std::ostream &os) const {
  os << "Node(";
  os << "type=" << getTypeName() << ", ";
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

const char *Node::getTypeName() const {
  return GetSymbolName(type);
}
