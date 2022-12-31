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

#ifndef SIMPLECC_DRIVER_WINDOWSDRIVER_H
#define SIMPLECC_DRIVER_WINDOWSDRIVER_H
#include "simplecc/Driver/DriverBase.h"

namespace simplecc {

/// This class abstracts the logic of running simplecc on Windows.
class WindowsDriver : public DriverBase {
  enum class OptSwitch {
    Yes, No,
  };
  enum class ProductKind {
    IR, Target, Result,
  };
  void runSixFiles();
  void runPrintASM(std::ostream &OS);
  void printFilename(std::ostream &OS, OptSwitch Opt, ProductKind Product);
  const char *getFilenamePrefix() const;
  const char *getExtension() const;
  void PressAnyKeyToContinue() const;
public:
  WindowsDriver() = default;
  int run(int argc, char **argv);
};
}
#endif //SIMPLECC_DRIVER_WINDOWSDRIVER_H