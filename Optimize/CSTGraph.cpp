#include "CSTGraph.h"

#include <llvm/Support/GraphWriter.h>

namespace simplecompiler {
void PrintAllNodes(Node *Root) {
  for (const Node *N : NodeIterator::GetIter(Root)) {
    Print(std::cout, N->GetTypeName(), N->GetValue(), N->GetLocation());
  }
}

void WriteCSTGraph(Node *Root, llvm::raw_ostream &os) {
  llvm::WriteGraph(os, Root);
}

}
