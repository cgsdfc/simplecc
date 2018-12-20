#include "simplecc/Driver/CommandLine.h"
#include "simplecc/Support/Print.h"
#include "simplecc/Driver/Driver.h"
#include <cassert>

using namespace simplecc;

static tclap::SwitchArg TokenizeSwitch("", "tokenize", "tokenize the input", false);
static tclap::SwitchArg PrintCSTSwitch("", "print-cst", "print the concrete syntax tree", false);
static tclap::SwitchArg PrintASTSwitch("", "print-ast", "print the abstract syntax tree", false);
static tclap::SwitchArg PrintBytecodeSwitch("", "print-bc", "print byte code sequence", false);
static tclap::SwitchArg PrintByteCodeModuleSwitch("", "print-module", "print byte code module", false);
static tclap::SwitchArg AssemblySwitch("", "asm", "emit MIPS assembly", false);

CommandLine::CommandLine() :
    Parser("Simple Compiler Debugging helper", ' ', "0.0.1"),
    InputArg("input", "input file (default to stdin)", false, "",
             "input-file"),
    OutputArg("o", "output", "output file (default to stdout)", false, "",
              "output-file") {
  Parser.add(InputArg);
  Parser.add(OutputArg);
  Switches.push_back(&TokenizeSwitch);
  Switches.push_back(&PrintCSTSwitch);
  Switches.push_back(&PrintASTSwitch);
  Switches.push_back(&PrintBytecodeSwitch);
  Switches.push_back(&PrintByteCodeModuleSwitch);
  Switches.push_back(&AssemblySwitch);
  Parser.xorAdd(Switches);
}

CommandLine::~CommandLine() = default;

int CommandLine::run(int Argc, char **Argv) {
  try {
    Parser.parse(Argc, Argv);
  } catch (tclap::ArgException &Exc) {
    PrintErrs(Exc.error(), "at argument", Exc.argId());
    return 1;
  }

  /// Create the Driver instance.
  TheDriver.reset(new Driver());
  TheDriver->setInputFile(InputArg.isSet() ? InputArg.getValue() : "-");
  TheDriver->setOutputFile(OutputArg.isSet() ? OutputArg.getValue() : "-");

  if (TokenizeSwitch.isSet()) {
    TheDriver->runPrintTokens();
  } else if (PrintCSTSwitch.isSet()) {
    TheDriver->runDumpCst();
  } else if (PrintASTSwitch.isSet()) {
    TheDriver->runDumpAst();
  } else if (PrintBytecodeSwitch.isSet()) {
    TheDriver->runPrintByteCode();
  } else if (PrintByteCodeModuleSwitch.isSet()) {
    TheDriver->runDumpByteCodeModule();
  } else if (AssemblySwitch.isSet()) {
    TheDriver->runAssembleMips();
  } else {
    assert(false && "Unhandled command line switch!");
  }
  return TheDriver->status();
}