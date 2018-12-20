#include <simplecc/Driver/CommandLine.h>

using namespace simplecc;

int main(int argc, char **argv) {
  return CommandLine().run(argc, argv);
}