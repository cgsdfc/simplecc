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