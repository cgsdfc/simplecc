#ifndef SIMPLECOMPILER_COMMANDLINE_H
#define SIMPLECOMPILER_COMMANDLINE_H
#include "simplecc/Driver/Driver.h"
#include <tclap/CmdLine.h>
#include <vector>

namespace simplecc {
namespace tclap = TCLAP;

class CommandLine {
  tclap::CmdLine Parser;
  tclap::UnlabeledValueArg<std::string> InputArg;
  tclap::ValueArg<std::string> OutputArg;
  std::vector<tclap::Arg *> Switches;
  Driver TheDriver;
public:
  CommandLine();
  ~CommandLine() = default;
  int run(int Argc, char **Argv);
};
}

#endif //SIMPLECOMPILER_COMMANDLINE_H
