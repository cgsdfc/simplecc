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

#ifndef SIMPLECC_SUPPORT_ERRORMANAGER_H
#define SIMPLECC_SUPPORT_ERRORMANAGER_H
#include "simplecc/Lex/Location.h"
#include "simplecc/Support/Print.h"
#include <iostream>
#include <utility>
#include <cassert>

namespace simplecc {
inline std::string Quote(const std::string &string) {
  return '\'' + string + '\'';
}

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

  /// TODO: these 2 overloads are too easy to be ambiguous.
  template <typename... Args> void Error(Location loc, Args &&... args) {
    getOuts() << getErrorType() << " at ";
    loc.FormatCompact(getOuts());
    getOuts() << " ";
    WriteLine(std::forward<Args>(args)...);
    increaseErrorCount();
  }

  template <typename... Args> void Error(Args &&... args) {
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