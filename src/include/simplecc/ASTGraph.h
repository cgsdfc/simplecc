#ifndef ASTGRAPH_H
#define ASTGRAPH_H
#include <iostream>

namespace llvm {
class raw_ostream;
}

namespace simplecc {
class Program;
}

namespace simplecc {
void PrintAllAstNodes(Program *P, std::ostream &os);
void WriteASTGraph(Program *P, llvm::raw_ostream &O);
} // namespace simplecc

#endif
