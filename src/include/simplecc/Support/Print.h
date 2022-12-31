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

#ifndef SIMPLECC_SUPPORT_PRINT_H
#define SIMPLECC_SUPPORT_PRINT_H

#include <iostream>
#include <utility>

namespace simplecc {
template <typename... Args> void Print(std::ostream &os, Args &&... args);

template <> inline void Print(std::ostream &os) { os << "\n"; }

template <typename Last> inline void Print(std::ostream &os, Last &&last) {
  os << std::forward<Last>(last) << "\n";
}

template <typename First, typename... Rest>
inline void Print(std::ostream &os, First &&first, Rest &&... rest) {
  os << std::forward<First>(first) << " ";
  Print(os, std::forward<Rest>(rest)...);
}

template <typename... Args> void PrintOuts(Args &&... args) {
  Print(std::cout, std::forward<Args>(args)...);
}

template <typename... Args> void PrintErrs(Args &&... args) {
  Print(std::cerr, std::forward<Args>(args)...);
}

class Printer {
  std::ostream &os;

public:
  explicit Printer(std::ostream &os) : os(os) {}

  template <typename... Args> void WriteLine(Args &&... args) {
    Print(os, std::forward<Args>(args)...);
  }

  /// Use this when Printer::WriteLine() becomes awkward.
  std::ostream &getOuts() { return os; }
};
} // namespace simplecc
#endif
