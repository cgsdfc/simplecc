#ifndef SIMPLECOMPILER_VISUALIZE_H
#define SIMPLECOMPILER_VISUALIZE_H
#include <iostream>
#include <llvm/Support/raw_ostream.h>

namespace simplecc {
class Program;
class Node;

void PrintAllAstNodes(Program *P, std::ostream &O);
void WriteASTGraph(Program *P, llvm::raw_ostream &O);

void PrintAllNodes(Node *Root);
void WriteCSTGraph(Node *Root, llvm::raw_ostream &O);
}
#endif //SIMPLECOMPILER_VISUALIZE_H
