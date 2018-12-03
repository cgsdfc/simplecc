#ifndef ASTGRAPH_H
#define ASTGRAPH_H
#include <iostream>

namespace llvm {
class raw_ostream;
}

namespace simplecompiler {
class Program;
}

namespace simplecompiler {
void PrintAllAstNodes(Program *P, std::ostream &os);
void WriteASTGraph(Program *P, llvm::raw_ostream &os);
}

#endif
