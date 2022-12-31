// MIT License
// 
// Copyright (c) 2018 Cong Feng.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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