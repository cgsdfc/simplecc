#ifndef SIMPLECC_VISUALIZE_VISUALIZE_H
#define SIMPLECC_VISUALIZE_VISUALIZE_H
#include <iostream>
#include <llvm/Support/raw_ostream.h>

namespace simplecc {
class Program;
class Node;
class AST;

void PrintAllASTNodes(Program *P, std::ostream &O);
void WriteASTGraph(Program *P, llvm::raw_ostream &O);

void PrintAllNodes(Node *Root);
void WriteCSTGraph(Node *Root, llvm::raw_ostream &O);
} // namespace simplecc
#endif // SIMPLECC_VISUALIZE_VISUALIZE_H
