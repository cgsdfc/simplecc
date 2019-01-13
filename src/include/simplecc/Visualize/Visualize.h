/// @file External interface of the Visualize module.
/// This module can visualize an AST or CST in the dot format, which can be rendered
/// in different image formats.
#ifndef SIMPLECC_VISUALIZE_VISUALIZE_H
#define SIMPLECC_VISUALIZE_VISUALIZE_H
#include <llvm/Support/raw_ostream.h>
#include <iostream>

namespace simplecc {
class ProgramAST;
class Node;
class AST;

/// This function prints a short description for each AST node in a program.
void PrintAllASTNodes(ProgramAST *P, std::ostream &O);

/// This function writes a DOT graph of an AST to output stream.
void WriteASTGraph(ProgramAST *P, llvm::raw_ostream &O);

/// This function prints a short description for each CST node in a program.
void PrintAllNodes(Node *Root);

/// This function write a DOT graph of a CST to output stream.
void WriteCSTGraph(Node *Root, llvm::raw_ostream &O);
} // namespace simplecc
#endif // SIMPLECC_VISUALIZE_VISUALIZE_H