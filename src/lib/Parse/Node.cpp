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

#include "simplecc/Parse/Node.h"
#include "simplecc/Lex/TokenInfo.h"
#include "simplecc/Support/ErrorManager.h"
#include <simplecc/Parse/ParseTreePrinter.h>
#include <algorithm> // for_each
#include <memory>    // default_delete

using namespace simplecc;

Node::~Node() {
  std::for_each(Children.begin(), Children.end(), std::default_delete<Node>());
}

void Node::Format(std::ostream &O) const { ParseTreePrinter(O).Print(*this); }

void Node::dump() const {
  return Format(std::cerr);
}

const char *Node::getTypeName() const { return TokenInfo::getSymbolName(Type); }

Node::Node(Symbol Ty, std::string Val, Location L)
    : Type(Ty), Value(std::move(Val)), Children(), Loc(L) {}