#include "simplecc/Parse/Node.h"
#include "simplecc/Support/Print.h"
#include "simplecc/Visualize/NodeIterator.h"
#include "simplecc/Visualize/Visualize.h"

#include <llvm/ADT/GraphTraits.h>
#include <llvm/Support/GraphWriter.h>

namespace llvm {
using simplecc::NodeIterator;
using CSTGraphTy = NodeIterator::value_type;

/// Specialized GraphTraits
template<> struct GraphTraits<CSTGraphTy> {
  using NodeRef = CSTGraphTy;
  using nodes_iterator = NodeIterator;
  using ChildIteratorType = NodeIterator::ChildIteratorType;

  static nodes_iterator nodes_begin(const CSTGraphTy &G) {
    return simplecc::NodeIterator(G);
  }

  static nodes_iterator nodes_end(const CSTGraphTy &) {
    return simplecc::NodeIterator();
  }

  static ChildIteratorType child_begin(NodeRef N) { return N->begin(); }
  static ChildIteratorType child_end(NodeRef N) { return N->end(); }
};

template<> struct DOTGraphTraits<CSTGraphTy> : DefaultDOTGraphTraits {
  explicit DOTGraphTraits(bool simple = false)
      : DefaultDOTGraphTraits(simple) {}

  static std::string getGraphName(const CSTGraphTy &) {
    return "Concrete Syntax Tree";
  }

  std::string getNodeLabel(const void *NodeRef, const CSTGraphTy &) {
    auto N = static_cast<CSTGraphTy>(const_cast<void *>(NodeRef));
    return N->getTypeName();
  }

  static std::string getNodeDescription(const void *NodeRef,
                                        const CSTGraphTy &) {
    auto N = static_cast<CSTGraphTy>(const_cast<void *>(NodeRef));
    return N->getValue();
  }
};
} // namespace llvm

namespace simplecc {

void PrintAllNodes(Node *Root) {
  for (const Node *N : makeNodeRange(Root)) {
    Print(std::cout, N->getTypeName(), N->getValue(), N->getLocation());
  }
}

void WriteCSTGraph(Node *Root, llvm::raw_ostream &O) {
  llvm::WriteGraph(O, Root);
}
} // namespace simplecc
