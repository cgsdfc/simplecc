#include "simplecc/Support/Print.h"
#include <simplecc/Driver/Driver.h>

#include <string>
#include <iostream> // for cin
#include <memory>   // for unique_ptr

using namespace simplecc;

static inline void PressAnyKeyToContinue() {
  char Chr;
  PrintErrs("Press any key to continue");
  std::cin >> Chr;
}

int main(int argc, char **argv) {
  std::string Filename;
  /// Prompt the user for an input filename.
  PrintErrs("Please input a filename:");
  std::getline(std::cin, Filename);

  std::unique_ptr<Driver> TheDriver(new Driver());
  TheDriver->setInputFile(Filename);
  TheDriver->setOutputFile("-");
  TheDriver->runAssembleMips();

  /// To be Windows-friendly.
  PressAnyKeyToContinue();
  return TheDriver->status();
}