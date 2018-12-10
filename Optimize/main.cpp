#include "Pass.h"
#include "error.h"

#include <cstring>
#include <fstream>
#include <tclap/CmdLine.h>

using namespace TCLAP;
using namespace simplecompiler;

class CommandLine {
  CmdLine Parser;
  PassManager PM;
  std::vector<Arg *> Args;
  UnlabeledValueArg<String> InputArg;
  ValueArg<String> OutputArg;
  ErrorManager EM;

public:
  CommandLine()
      : Parser("Simple Compiler Debugging helper", ' ', "0.0.1"),
        InputArg("input", "input file (default to stdin)", false, "",
                 "input-file", Parser),
        OutputArg("o", "output", "output file (default to stdout)", false, "",
                  "output-file", Parser) {

    for (auto &&Pair : getGlobalRegistry()) {
      auto &&PI = Pair.second;
      if (!std::strlen(PI->getName()))
        continue;
      Args.push_back(
          new SwitchArg("", PI->getName(), PI->getDescription(), false));
    }
    Parser.xorAdd(Args);
  }

  ~CommandLine() {
    std::for_each(std::begin(Args), std::end(Args), [](Arg *A) { delete A; });
  }

  int run(int argc, char **argv) {
    try {
      Parser.parse(argc, argv);
    } catch (TCLAP::ArgException &Exc) {
      EM.Error(Exc.error(), "at argument", Exc.argId());
      return 1;
    }

    String InputFile = InputArg.isSet() ? InputArg.getValue() : "";
    String OutputFile = OutputArg.isSet() ? OutputArg.getValue() : "";

    if (!PM.setInputFile(InputFile)) {
      EM.FileReadError(InputFile);
      return 1;
    }

    if (!PM.setOutputFile(OutputFile)) {
      EM.FileWriteError(OutputFile);
      return 1;
    }

    /// Find the argument selected by the user.
    auto ArgIter = std::find_if(std::begin(Args), std::end(Args),
                                [](Arg *V) { return V->isSet(); });
    assert(ArgIter != Args.end() && "At one argument must be given");
    Arg *SelectedArg = *ArgIter;

    /// Find the PassInfo entry corresponding to the argument.
    PassInfo *PI = getGlobalRegistry().getPassInfo(SelectedArg->getName());
    /// Run the specific Pass.
    return !PM.run(PI->getID());
  }
};

int main(int argc, char **argv) { return CommandLine().run(argc, argv); }
