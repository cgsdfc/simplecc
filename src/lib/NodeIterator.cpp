#include "simplecc/NodeIterator.h"
#include "simplecc/Node.h"

using namespace simplecc;

/// Return the next Node or nullptr.
Node *NodeIteratorImpl::getNext() {
  if (TheStack.empty())
    return nullptr;
  Node *Tos = TheStack.top();
  TheStack.pop();
  for (Node *Child : Tos->getChildren()) {
    TheStack.push(Child);
  }
  return Tos;
}