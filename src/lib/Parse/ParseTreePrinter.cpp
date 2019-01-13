#include <simplecc/Lex/TokenInfo.h>
#include <simplecc/Parse/ParseTreePrinter.h>

using namespace simplecc;

/// (NAME: Var)
/// (STRING: "a")
/// (ENDMARKER)
void ParseTreePrinter::printTerminalNode(const Node &N) {
  /// ENDMARKER don't have a valid value.
  if (N.getType() == Symbol::ENDMARKER)
    return;
  /// Add quotes to OP value so that it doesn't mix with lisp-style parentheses.
  if (N.getType() == Symbol::OP) {
    OS << ": '" << N.getValue() << "'";
    return;
  }
  OS << ": " << N.getValue();
}

/// Non terminal nodes are valueless nodes. They have a list of children, each
/// of which will be printed on its own line if NumChildren is more than one.
/// (term:
///   (factor: ...),
///   (OP: *),
///   (factor: ...))
///
/// (stmt: (return_stmt: (NAME: return))
void ParseTreePrinter::printNonTerminalNode(const Node &N) {
  assert(N.getNumChildren() && "NonTerminal Node must have a least one child");
  OS << ": ";
  if (N.getNumChildren() == 1) {
    printNode(*(N.getFirstChild()));
    return;
  }
  OS << "\n";
  increaseIndentLevel();
  printNodeList(N.getChildren());
  decreaseIndentLevel();
}

/// Print a list of Nodes, each on its own line with indent.
void ParseTreePrinter::printNodeList(const Node::ChildrenListType &NodeList) {
  for (auto I = NodeList.begin(), E = NodeList.end(); I != E; ++I) {
    printIndent();
    printNode(*(*I));
    if (I != E - 1)
      OS << ",\n";
  }
}

void ParseTreePrinter::printNode(const Node &N) {
  OS << "(" << N.getTypeName();
  if (TokenInfo::IsTerminal(N.getType())) {
    printTerminalNode(N);
  } else {
    assert(TokenInfo::IsNonTerminal(N.getType()));
    printNonTerminalNode(N);
  }
  OS << ")";
}