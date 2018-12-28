#include "simplecc/Visualize/Visualize.h"
#include "simplecc/Visualize/ASTGraph.h"
#include "simplecc/Visualize/AstPrettyPrinter.h"
#include "simplecc/Support/Print.h"

namespace simplecc {
/// Print all ast nodes from a root.
void PrintAllAstNodes(Program *P, std::ostream &O) {
  AstGraph Graph(P);
  for (auto AR : Graph.nodes()) {
    Print(O, AR->getClassName(), AR->getLocation());
  }
}

/// Pretty print an AST node.
void PrettyPrintAST(const AST &A, std::ostream &O) {
  AstPrettyPrinter(O).PrettyPrint(&A);
  O << "\n";
}
}