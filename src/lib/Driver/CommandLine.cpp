#include "simplecc/Driver/CommandLine.h"
#include "simplecc/Driver/Driver.h"

using namespace simplecc;
#define HANDLE_COMMAND(Name, Arg, Description)                                 \
  static tclap::SwitchArg Name##Switch("", Arg, Description, false);
#include "simplecc/Driver/Driver.def"

CommandLine::CommandLine()
    : Parser("Simple Compiler", ' ', "2.0"),
      InputArg("input", "input file (default to stdin)", false, "",
               "input-file"),
      OutputArg("o", "output", "output file (default to stdout)", false, "",
                "output-file") {
  Parser.add(InputArg);
  Parser.add(OutputArg);

#define HANDLE_COMMAND(Name, Arg, Description)                                 \
  Switches.push_back(&Name##Switch);
#include "simplecc/Driver/Driver.def"

  Parser.xorAdd(Switches);
}

int CommandLine::run(int Argc, char **Argv) {
  try {
    Parser.parse(Argc, Argv);
  } catch (tclap::ArgException &Exc) {
    PrintErrs(Exc.error(), "at argument", Exc.argId());
    return 1;
  }

  /// Create the Driver instance.
  std::unique_ptr<Driver> TheDriver(new Driver());
  TheDriver->setInputFile(InputArg.isSet() ? InputArg.getValue() : "-");
  TheDriver->setOutputFile(OutputArg.isSet() ? OutputArg.getValue() : "-");

#define HANDLE_COMMAND(Name, Arg, Description)                                 \
  if (Name##Switch.isSet()) {                                                  \
    TheDriver->run##Name();                                                    \
    return TheDriver->status();                                                \
  }
#include "simplecc/Driver/Driver.def"

  assert(false && "Unhandled command line switch!");
}
