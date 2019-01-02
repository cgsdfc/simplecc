#include "simplecc/Parse/Node.h"
#include "simplecc/Lex/TokenInfo.h"
#include "simplecc/Support/ErrorManager.h"
#include <simplecc/Parse/ParseTreePrinter.h>
#include <algorithm> // for_each
#include <memory>    // default_delete
#include <simplecc/Parse/Node.h>

using namespace simplecc;

Node::~Node() {
  std::for_each(Children.begin(), Children.end(), std::default_delete<Node>());
}

void Node::Format(std::ostream &O) const { ParseTreePrinter(O).Print(*this); }

void Node::dump() const {
  return Format(std::cerr);
}

const char *Node::getTypeName() const { return getSymbolName(Type); }

Node::Node(Symbol Ty, std::string Val, Location L)
    : Type(Ty), Value(std::move(Val)), Children(), Loc(L) {}