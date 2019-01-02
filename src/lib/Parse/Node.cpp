#include "simplecc/Parse/Node.h"
#include "simplecc/Support/ErrorManager.h"
#include "simplecc/Lex/TokenInfo.h"
#include <simplecc/Parse/ParseTreePrinter.h>

#include <algorithm> // for_each
#include <memory> // default_delete

using namespace simplecc;

Node::~Node() {
  std::for_each(Children.begin(), Children.end(), std::default_delete<Node>());
}

void Node::Format(std::ostream &O) const {
  ParseTreePrinter(O).Print(*this);
}

const char *Node::getTypeName() const { return getSymbolName(Type); }