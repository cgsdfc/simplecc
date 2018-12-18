#include "simplecc/Node.h"
#include "simplecc/ErrorManager.h"

#include <algorithm> // for_each

using namespace simplecc;

Node::~Node() {
  std::for_each(Children.begin(), Children.end(), [](Node *N) { delete N; });
}

String Node::FormatValue() const {
  return Type != Symbol::ENDMARKER && Value.empty() ? "None" : Quote(Value);
}

void Node::Format(std::ostream &O) const {
  O << "Node(";
  O << "type=" << getTypeName() << ", ";
  O << "value=" << FormatValue() << ", ";
  O << Loc << ", ";
  O << "children=";
  if (Children.empty()) {
    O << "None)";
  } else {
    O << "[";
    for (int i = 0; i < Children.size(); i++) {
      Children[i]->Format(O);
      if (i != Children.size() - 1)
        O << ", ";
    }
    O << "])";
  }
}

const char *Node::getTypeName() const { return getSymbolName(Type); }