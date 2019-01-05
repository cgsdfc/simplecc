#include "simplecc/Driver/Driver.h"
#include <memory> // unique_ptr

using namespace simplecc;

int main(int argc, char **argv) {
  std::unique_ptr<Driver> D(new Driver());
  return D->run(argc, argv);
}