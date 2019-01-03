#include "simplecc/Driver/WindowsDriver.h"

using namespace simplecc;

int main(int argc, char **argv) {
  std::unique_ptr<WindowsDriver> TheDriver(new WindowsDriver());
  return TheDriver->run(argc, argv);
}