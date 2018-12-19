#include "simplecc/Visualize/AstIterator.h"
#include "simplecc/Visualize/ASTGraph.h"

using namespace simplecc;

AstIteratorImpl::AstIteratorImpl(Program *Ptr, AstGraph *G)
    : TheStack(), Parent(G) {
  auto AR = Parent->getNodeOrCreate(Ptr);
  TheStack.push_back(AR);
}

AstRef *AstIteratorImpl::getNext() {
  if (TheStack.empty()) {
    return nullptr;
  }
  auto TOS = TheStack.back();
  TheStack.pop_back();
  const auto &LazyEdges = Parent->getEdgeOrCreate(*TOS);
  std::copy(LazyEdges.begin(), LazyEdges.end(), std::back_inserter(TheStack));
  return TOS;
}