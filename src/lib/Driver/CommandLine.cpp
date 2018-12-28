#include "simplecc/Driver/CommandLine.h"
#include "simplecc/Driver/Driver.h"
#include "simplecc/Support/Print.h"

using namespace simplecc;
// TODO: use Driver.def to simplify this.

static tclap::SwitchArg TokenizeSwitch("", "tokenize", "tokenize the input",
                                       false);
static tclap::SwitchArg PrintCSTSwitch("", "print-cst",
                                       "print the concrete syntax tree", false);
static tclap::SwitchArg PrintASTSwitch("", "print-ast",
                                       "pretty print the abstract syntax tree", false);
static tclap::SwitchArg
    PrintBytecodeSwitch("", "print-school-ir",
                        "print IR in the format required by school", false);
static tclap::SwitchArg
    PrintByteCodeModuleSwitch("", "print-bc-ir",
                              "print IR in the byte code form", false);
static tclap::SwitchArg AssemblySwitch("", "asm", "emit MIPS assembly", false);
static tclap::SwitchArg CheckOnlySwitch("", "check-only",
                                        "merely perform checks on the input",
                                        false);

#ifdef SIMPLE_COMPILER_USE_LLVM
static tclap::SwitchArg AstGraphSwitch("", "ast-graph",
                                       "print the dot file for the AST", false);
static tclap::SwitchArg CstGraphSwitch("", "cst-graph",
                                       "print the dot file for the CST", false);
static tclap::SwitchArg EmitLLVMIRSwitch("", "emit-llvm", "emit LLVM IR",
                                         false);
#endif

CommandLine::CommandLine()
    : Parser("Simple Compiler", ' ', "2.0"),
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
  Switches.push_back(&CheckOnlySwitch);

#ifdef SIMPLE_COMPILER_USE_LLVM
  Switches.push_back(&AstGraphSwitch);
  Switches.push_back(&CstGraphSwitch);
  Switches.push_back(&EmitLLVMIRSwitch);
#endif

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

  if (TokenizeSwitch.isSet()) {
    TheDriver->runPrintTokens();
  } else if (PrintCSTSwitch.isSet()) {
    TheDriver->runDumpCst();
  } else if (PrintASTSwitch.isSet()) {
    TheDriver->runPrettyPrintAST();
  } else if (PrintBytecodeSwitch.isSet()) {
    TheDriver->runPrintByteCode();
  } else if (PrintByteCodeModuleSwitch.isSet()) {
    TheDriver->runDumpByteCodeModule();
  } else if (AssemblySwitch.isSet()) {
    TheDriver->runAssembleMips();
  } else if (CheckOnlySwitch.isSet()) {
    TheDriver->runAnalysisOnly();
#ifdef SIMPLE_COMPILER_USE_LLVM
  } else if (CstGraphSwitch.isSet()) {
    TheDriver->runWriteCstGraph();
  } else if (AstGraphSwitch.isSet()) {
    TheDriver->runWriteAstGraph();
  } else if (EmitLLVMIRSwitch.isSet()) {
    TheDriver->runEmitLLVMIR();
#endif
  } else {
    assert(false && "Unhandled command line switch!");
  }
  return TheDriver->status();
}
