#ifndef ERROR_H
#define ERROR_H

#include "Print.h"
#include "tokenize.h"

class ErrorManager: private Printer {
  int error_count;

  template <typename... Args>
    inline void ErrorImpl(const char *etype, const Location &loc, Args &&... args) {
      WriteLine(etype, "at", loc.lineno, ":", std::forward<Args>(args)...);
      ++error_count;
    }

public:
  ErrorManager() : Printer(std::cerr), error_count(0) {}

  template <typename... Args> void Error(const Location &loc, Args &&... args) {
    ErrorImpl("Error:", loc, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void SyntaxError(const Location &loc, Args &&... args) {
    ErrorImpl("SyntaxError", loc, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void NameError(const Location &loc, Args &&... args) {
    ErrorImpl("NameError", loc, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void TypeError(const Location &loc, Args &&... args) {
    ErrorImpl("TypeError", loc, std::forward<Args>(args)...);
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
