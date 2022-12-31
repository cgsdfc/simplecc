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

#include "simplecc/Driver/WindowsDriver.h"
#include "simplecc/CodeGen/CodeGen.h"

using namespace simplecc;

const char *WindowsDriver::getFilenamePrefix() const {
  return u8"15061084_冯聪_";
}

const char *WindowsDriver::getExtension() const {
  return u8".txt";
}

void WindowsDriver::PressAnyKeyToContinue() const {
  char Chr;
  PrintErrs("Press any key to continue");
  std::cin >> Chr;
}

int WindowsDriver::run(int argc, char **argv) {
  std::string Filename;
  PrintErrs("Please input a filename:");
  std::getline(std::cin, Filename);

  setInputFile(Filename);
  setOutputFile("-");
  runSixFiles();
  PressAnyKeyToContinue();
  return status();
}

// 学号_姓名_优化前中间代码.TXT
// 学号_姓名_优化后中间代码.TXT
// 学号_姓名_优化前目标代码.TXT
// 学号_姓名_优化后目标代码.TXT
// 学号_姓名_优化前运行结果.TXT
// 学号_姓名_优化后运行结果.TXT
void WindowsDriver::runSixFiles() {
  if (runAnalyses())
    return;
  auto OS = getStdOstream();
  assert(OS && "std::cout must be available");

  // Before Optimization IR
  printFilename(*OS, OptSwitch::No, ProductKind::IR);
  PrintByteCode(getProgram(), *OS);
  // Before Optimization Target
  printFilename(*OS, OptSwitch::No, ProductKind::Target);
  runPrintASM(*OS);
  printFilename(*OS, OptSwitch::No, ProductKind::Result);

  doTransform();
  printFilename(*OS, OptSwitch::Yes, ProductKind::IR);
  // After Optimization IR
  PrintByteCode(getProgram(), *OS);
  printFilename(*OS, OptSwitch::Yes, ProductKind::Target);
  // After Optimization Target
  runPrintASM(*OS);
  printFilename(*OS, OptSwitch::Yes, ProductKind::Result);
}

void WindowsDriver::printFilename(std::ostream &OS, OptSwitch Opt, ProductKind Product) {
  OS << getFilenamePrefix();
  switch (Opt) {
  case OptSwitch::Yes: OS << u8"优化后";
    break;
  case OptSwitch::No:OS << u8"优化前";
    break;
  }
  switch (Product) {
  case ProductKind::IR: OS << u8"中间代码";
    break;
  case ProductKind::Target: OS << u8"目标代码";
    break;
  case ProductKind::Result: OS << u8"运行结果";
    break;
  }
  OS << getExtension() << "\n";
}

void WindowsDriver::runPrintASM(std::ostream &OS) {
  doCodeGen();
  doAssemble(OS);
}