#ifndef SIMPLECC_VISUALIZE_VISUALIZE_H
#define SIMPLECC_VISUALIZE_VISUALIZE_H
#include <llvm/Support/raw_ostream.h>
#include <iostream>

namespace simplecc {
class ProgramAST;
class Node;
class AST;

void PrintAllASTNodes(ProgramAST *P, std::ostream &O);
void WriteASTGraph(ProgramAST *P, llvm::raw_ostream &O);

void PrintAllNodes(Node *Root);
void WriteCSTGraph(Node *Root, llvm::raw_ostream &O);
} // namespace simplecc
#endif // SIMPLECC_VISUALIZE_VISUALIZE_H