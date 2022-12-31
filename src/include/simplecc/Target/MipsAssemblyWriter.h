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

#ifndef SIMPLECC_TARGET_MIPSASSEMBLYWRITER_H
#define SIMPLECC_TARGET_MIPSASSEMBLYWRITER_H
#include "simplecc/Support/Print.h"
#include "simplecc/Target/LocalContext.h"
#include <iostream>

namespace simplecc {
class ByteCodeFunction;
class ByteCodeModule;

class MipsAssemblyWriter {
  // Return the total bytes consumed by local objects, including
  // variables, arrays and formal arguments.
  int getLocalObjectsInBytes(const ByteCodeFunction &TheFunction) const;

  /// Write data segment -- strings, arrays and variables.
  void WriteData(Printer &W, const ByteCodeModule &Module);
  /// Write text segment -- the bundle of functions.
  void WriteText(Printer &W, const ByteCodeModule &Module);
  /// Write prologue for TheFunction.
  void WritePrologue(Printer &W, const ByteCodeFunction &TheFunction);
  /// Write epilogue for TheFunction.
  void WriteEpilogue(Printer &W, const ByteCodeFunction &TheFunction);
  /// Write one ByteCodeFunction to MIPS assembly.
  void WriteFunction(Printer &W, const ByteCodeFunction &TheFunction);

public:
  MipsAssemblyWriter() = default;
  ~MipsAssemblyWriter() = default;

  /// Write one ByteCodeModule to output translating to MIPS.
  void Write(const ByteCodeModule &M, std::ostream &O);

private:
  LocalContext TheContext;
};

} // namespace simplecc

#endif