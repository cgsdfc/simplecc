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

#ifndef SIMPLECC_PARSE_PARSETREEPRINTER_H
#define SIMPLECC_PARSE_PARSETREEPRINTER_H
#include "simplecc/Support/IndentAwarePrinter.h"
#include <simplecc/Parse/Node.h>

namespace simplecc {
/// ParseTreePrinter pretty-prints the parse tree.
class ParseTreePrinter : IndentAwarePrinter<ParseTreePrinter> {
  void printTerminalNode(const Node &N);
  void printNonTerminalNode(const Node &N);
  void printNodeList(const Node::ChildrenListType &NodeList);
  void printNode(const Node &N);

public:
  explicit ParseTreePrinter(std::ostream &OS) : OS(OS) {}
  void Print(const Node &N) { printNode(N); }

private:
  std::ostream &OS;
  friend IndentAwarePrinter;
  std::ostream &getOstream() { return OS; }
};

} // namespace simplecc
#endif // SIMPLECC_PARSE_PARSETREEPRINTER_H