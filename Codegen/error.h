#ifndef ERROR_H
#define ERROR_H

#include "tokenize.h"

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

template <typename... Args>
inline void ErrorImpl(const char *etype, const Location &loc, Args &&... args) {
  Print(std::cerr, etype, "at", loc.lineno, ":", std::forward<Args>(args)...);
}

template <typename... Args>
inline void Error(const Location &loc, Args &&... args) {
  return ErrorImpl("Error", loc, std::forward<Args>(args)...);
}

class ErrorManager {
  int error_count;

public:
  ErrorManager() : error_count(0) {}

  template <typename... Args> void Error(const Location &loc, Args &&... args) {
    ::Error(loc, std::forward<Args>(args)...);
    ++error_count;
  }

  template <typename... Args>
  void SyntaxError(const Location &loc, Args &&... args) {
    ErrorImpl("SyntaxError", loc, std::forward<Args>(args)...);
    ++error_count;
  }

  template <typename... Args>
  void NameError(const Location &loc, Args &&... args) {
    ErrorImpl("NameError", loc, std::forward<Args>(args)...);
    ++error_count;
  }

  template <typename... Args>
  void TypeError(const Location &loc, Args &&... args) {
    ErrorImpl("TypeError", loc, std::forward<Args>(args)...);
    ++error_count;
  }

  template <typename... Args>
  [[noreturn]] void InternalError(const Location &loc, Args &&... args) {
    ErrorImpl("InternalError", loc, std::forward<Args>(args)...);
    std::abort();
  }

  int GetErrorCount() const {
    return error_count;
  }

  bool IsOk(int prev_count = 0) const { return prev_count == GetErrorCount(); }
};

inline String Quote(const String &string) { return '\'' + string + '\''; }

#endif
