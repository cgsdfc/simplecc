#include "simplecc/Visualize/Visualize.h"
#include "simplecc/Visualize/ASTGraph.h"
#include "simplecc/Support/Print.h"

namespace simplecc {
/// Print all ast nodes from a root.
void PrintAllAstNodes(Program *P, std::ostream &O) {
  AstGraph Graph(P);
  for (auto AR : Graph.nodes()) {
    Print(O, AR->getClassName(), AR->getLocation());
  }
}
}