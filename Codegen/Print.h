#ifndef PRINT_H
#define PRINT_H
#include <iostream>

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

class Printer {
  std::ostream &os;

public:
  Printer(std::ostream &os) : os(os) {}

  template <typename... Args> void WriteLine(Args &&... args) {
    Print(os, std::forward<Args>(args)...);
  }
};

#endif
