#ifndef ERROR_H
#define ERROR_H

#include "Print.h"
#include "TokenInfo.h"

#include <iostream>
#include <sstream>
#include <utility>

namespace simplecompiler {
inline String Quote(const String &string) { return '\'' + string + '\''; }

class ErrorManager : private Printer {
  int error_count;

  template <typename... Args>
  void ErrorWithLocation(const char *etype, const Location &loc,
                         Args &&... args) {
    std::ostringstream os;
    loc.FormatCompact(os);
    WriteLine(etype, "at", os.str(), std::forward<Args>(args)...);
    ++error_count;
  }

public:
  ErrorManager() : Printer(std::cerr), error_count(0) {}

  template <typename... Args> void Error(Args &&... args) {
    WriteLine("Error:", std::forward<Args>(args)...);
    ++error_count;
  }

  void FileReadError(const String &filename) {
    WriteLine("FileReadError:", "File", Quote(filename), "does not exist");
    ++error_count;
  }

  void FileWriteError(const String &filename) {
    WriteLine("FileWriteError: File", Quote(filename), "cannot be written to");
    ++error_count;
  }

  template <typename... Args>
  void SyntaxError(const Location &loc, Args &&... args) {
    ErrorWithLocation("SyntaxError", loc, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void NameError(const Location &loc, Args &&... args) {
    ErrorWithLocation("NameError", loc, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void TypeError(const Location &loc, Args &&... args) {
    ErrorWithLocation("TypeError", loc, std::forward<Args>(args)...);
  }

  template <typename... Args>
  [[noreturn]] void InternalError(Args &&... args) {
    WriteLine("InternalError", std::forward<Args>(args)...);
    std::abort();
  }

  int GetErrorCount() const {
    return error_count;
  }

  bool IsOk(int prev_count = 0) const { return prev_count == GetErrorCount(); }
};

} // namespace simplecompiler
#endif
