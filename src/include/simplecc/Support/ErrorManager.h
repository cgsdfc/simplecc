#ifndef ERROR_MANAGER_H
#define ERROR_MANAGER_H

#include "Print.h"
#include "simplecc/Lex/TokenInfo.h"

#include <iostream>
#include <sstream>
#include <utility>
#include <cassert>

namespace simplecc {
inline String Quote(const String &string) { return '\'' + string + '\''; }

class ErrorManager : private Printer {
  int ErrorCount = 0;
  const char *ErrorType;
public:
  ErrorManager(const char *ET = nullptr) : Printer(std::cerr) {
    setErrorType(ET);
  }

  void setErrorType(const char *ET) {
    ErrorType = ET == nullptr ? "Error" : ET;
  }

  const char *getErrorType() const {
    assert(ErrorType && "ErrorType not set!");
    return ErrorType;
  }

  template<typename... Args>
  void Error(const Location &loc, Args &&... args) {
    getOuts() << getErrorType() << " at ";
    loc.FormatCompact(getOuts());
    getOuts() << " ";
    WriteLine(std::forward<Args>(args)...);
   increaseErrorCount();
  }

  template <typename ... Args>
  void Error(Args &&... args) {
    getOuts() << getErrorType() << ": ";
    WriteLine(std::forward<Args>(args)...);
    increaseErrorCount();
  }

  void clear() { ErrorCount = 0; }

  int getErrorCount() const { return ErrorCount; }
  void increaseErrorCount() { ++ErrorCount; }

  bool IsOk(int Prev = 0) const { return Prev == getErrorCount(); }
};

} // namespace simplecc
#endif
