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