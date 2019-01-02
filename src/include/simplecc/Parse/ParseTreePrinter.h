#ifndef SIMPLECC_PARSE_PARSETREEPRINTER_H
#define SIMPLECC_PARSE_PARSETREEPRINTER_H
#include "simplecc/Support/IndentAwarePrinter.h"
#include <simplecc/Parse/Node.h>

namespace simplecc {
/// This class pretty prints the parse tree.
class ParseTreePrinter : IndentAwarePrinter<ParseTreePrinter> {
  void printTerminalNode(const Node &N);
  void printNonTerminalNode(const Node &N);
  void printNodeList(const Node::ChildrenListT &NodeList);
  void printNode(const Node &N);

public:
  ParseTreePrinter(std::ostream &OS) : OS(OS) {}
  void Print(const Node &N) { printNode(N); }

private:
  std::ostream &OS;
  friend IndentAwarePrinter;
  std::ostream &getOS() { return OS; }
};

} // namespace simplecc
#endif // SIMPLECC_PARSE_PARSETREEPRINTER_H