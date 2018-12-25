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
