#ifndef PRINT_H
#define PRINT_H
#include <iostream>

namespace simplecompiler {
template <typename... Args> void Print(std::ostream &os, Args &&... args);

template <> inline void Print(std::ostream &os) { os << "\n"; }

template <typename Last> inline void Print(std::ostream &os, Last &&last) {
  os << last << "\n";
}

template <typename First, typename... Rest>
inline void Print(std::ostream &os, First &&first, Rest &&... rest) {
  os << first << " ";
  Print(os, rest...);
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
  Printer(std::ostream &os) : os(os) {}

  template <typename... Args> void WriteLine(Args &&... args) {
    Print(os, std::forward<Args>(args)...);
  }
};
} // namespace simplecompiler
#endif
