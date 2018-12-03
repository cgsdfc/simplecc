#ifndef CST_GRAPH_H
#define CST_GRAPH_H

#include "Node.h"
#include "Print.h"

#include <llvm/ADT/GraphTraits.h>
#include <stack>
#include <cassert>

namespace simplecompiler {
class NodeIteratorImpl {
  std::stack<Node*> Stack;
public:
  NodeIteratorImpl(Node *R): Stack() {
    assert(R && "Cannot iterate a null Node");
    Stack.push(R);
  }

  Node *GetNext() {
    if (Stack.empty()) return nullptr;
    Node *Tos = Stack.top();
    Stack.pop();
    for (Node *Child : Tos->children) {
      Stack.push(Child);
    }
    return Tos;
  }

  static void PrintAllNodes(Node *Root) {
    NodeIteratorImpl Iter(Root);
    while (auto N = Iter.GetNext()) {
      Print(std::cout, N->GetTypeName(), N->GetValue(), N->GetLocation());
    }
  }

};

}

#endif
