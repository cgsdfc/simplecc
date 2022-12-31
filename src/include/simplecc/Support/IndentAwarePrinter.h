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

#ifndef SIMPLECC_SUPPORT_INDENTAWAREPRINTER_H
#define SIMPLECC_SUPPORT_INDENTAWAREPRINTER_H
#include <iostream>

namespace simplecc {
// use combination:
// class IndentationPrinter {
// public:
//   void indent();
//   void dedent();
//   void printIndent(std::ostream &OS) const;
//   void printNewline(std::ostream &OS) const;
// private:
//   unsigned IndentLevel = 0;
// };
// TODO: fix this mutually dependent strange class.

/// IndentAwarePrinter is a CRTP Mixin that provides means to control
/// the indent level in each line to be printed. Subclass
/// should provide a getOS() method for this to work.
template <typename Derive> class IndentAwarePrinter {
public:
  void increaseIndentLevel() { ++IndentLevel; }
  void decreaseIndentLevel() { --IndentLevel; }
  unsigned getIndentLevel() const { return IndentLevel; }
  void printIndent() {
    for (unsigned I = 0, E = getIndentLevel(); I < E; I++) {
      static_cast<Derive *>(this)->getOstream() << "  ";
    }
  }

private:
  unsigned IndentLevel = 0;
};

} // namespace simplecc
#endif // SIMPLECC_SUPPORT_INDENTAWAREPRINTER_H