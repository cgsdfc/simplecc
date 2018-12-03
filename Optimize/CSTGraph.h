#ifndef CST_GRAPH_H
#define CST_GRAPH_H

namespace llvm {
class raw_ostream;
}

namespace simplecompiler {
class Node;
void PrintAllNodes(Node *Root);
void WriteCSTGraph(Node *Root, llvm::raw_ostream &os);
}

#endif
