#ifndef SIMPLECOMPILER_COMMANDLINE_H
#define SIMPLECOMPILER_COMMANDLINE_H
#include <tclap/CmdLine.h>
#include <vector>
#include <memory>

namespace simplecc {
namespace tclap = TCLAP;
class Driver;

class CommandLine {
  tclap::CmdLine Parser;
  tclap::UnlabeledValueArg<std::string> InputArg;
  tclap::ValueArg<std::string> OutputArg;
  std::vector<tclap::Arg *> Switches;
  std::unique_ptr<Driver> TheDriver;
public:
  CommandLine();
  ~CommandLine();
  int run(int Argc, char **Argv);
};
}

#endif //SIMPLECOMPILER_COMMANDLINE_H
