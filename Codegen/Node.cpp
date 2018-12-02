#include "Node.h"
#include "error.h"

using namespace simplecompiler;

Node::~Node() {
  for (auto child : children)
    delete child;
}

String Node::FormatValue() const {
  return type != Symbol::ENDMARKER && value.empty() ? "None" : Quote(value);
}

void Node::Format(std::ostream &os) const {
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
