#ifdef _MSC_VER
#include "simplecc/Driver/WindowsDriver.h"
using DriverTy = simplecc::WindowsDriver;
#else
#include "simplecc/Driver/Driver.h"
using DriverTy = simplecc::Driver;
#endif // _MSC_VER

#include <memory> // unique_ptr

int main(int argc, char **argv) {
  std::unique_ptr<DriverTy> D(new DriverTy());
  return D->run(argc, argv);
}