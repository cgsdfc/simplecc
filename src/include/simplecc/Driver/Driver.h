#ifndef SIMPLECC_DRIVER_DRIVER_H
#define SIMPLECC_DRIVER_DRIVER_H
#include "simplecc/Driver/DriverBase.h"

namespace llvm {
class raw_ostream;
}

namespace simplecc {
class Node;

class Driver : public DriverBase {
  std::unique_ptr<Node> runBuildCST();
public:
  Driver() = default;

#define HANDLE_COMMAND(Name, Arg, Description) void run##Name();
#include "simplecc/Driver/Driver.def"

  void runDumpSymbolTable();

#ifdef SIMPLE_COMPILER_USE_LLVM
  std::unique_ptr<llvm::raw_ostream> getLLVMRawOstream();
#endif
};

} // namespace simplecc
#endif // SIMPLECC_DRIVER_DRIVER_H