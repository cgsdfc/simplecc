#ifndef SIMPLECC_DRIVER_DRIVER_H
#define SIMPLECC_DRIVER_DRIVER_H
#include "simplecc/Driver/DriverBase.h"

namespace llvm {
class raw_ostream;
}

namespace simplecc {

class Driver : public DriverBase {
  std::unique_ptr<Node> runBuildCST();
  void runDumpSymbolTable();
#define HANDLE_COMMAND(Name, Arg, Description) void run##Name();
#include "simplecc/Driver/Driver.def"
#ifdef SIMPLE_COMPILER_USE_LLVM
  std::unique_ptr<llvm::raw_ostream> getLLVMRawOstream();
#endif

public:
  Driver() = default;
  int run(int argc, char **argv);
};

} // namespace simplecc
#endif // SIMPLECC_DRIVER_DRIVER_H