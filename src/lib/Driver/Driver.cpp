#include "simplecc/Driver/Driver.h"
#include "simplecc/CodeGen/Compile.h"
#include "simplecc/Lex/Tokenize.h"
#include "simplecc/Target/Assemble.h"
#include "simplecc/Transform/Transform.h"
#include <tclap/CmdLine.h>

#ifdef SIMPLE_COMPILER_USE_LLVM
#include "simplecc/LLVM/EmitLLVM.h"
#include "simplecc/Visualize/Visualize.h"
#endif

using namespace simplecc;

#ifdef SIMPLE_COMPILER_USE_LLVM
void Driver::runEmitLLVMIR() {
  if (runAnalyses())
    return;
  auto OS = getLLVMRawOstream();
  if (!OS)
    return;
  if (CompileToLLVMIR(getProgram(), getSymbolTable(), *OS)) {
    getEM().increaseErrorCount();
  }
}

void Driver::runWriteASTGraph() {
  if (runAnalyses())
    return;
  auto OS = getLLVMRawOstream();
  if (!OS)
    return;
  WriteASTGraph(getProgram(), *OS);
}

void Driver::runWriteCSTGraph() {
  auto TheCST = runBuildCST();
  if (!TheCST)
    return;
  auto OS = getLLVMRawOstream();
  if (!OS)
    return;
  WriteCSTGraph(TheCST.get(), *OS);
}

std::unique_ptr<llvm::raw_ostream> Driver::getLLVMRawOstream() {
  std::error_code EC;
  auto OS = llvm::make_unique<llvm::raw_fd_ostream>(getOutputFile(), EC);
  if (EC) {
    // Destroy the raw_fd_ostream as told by their doc.
    OS.release();
    getEM().setErrorType("FileWriteError");
    getEM().Error(getOutputFile());
    return nullptr;
  }
  return std::move(OS);
}
#endif // SIMPLE_COMPILER_USE_LLVM

std::unique_ptr<Node> Driver::runBuildCST() {
  if (runTokenize())
    return nullptr;
  auto CST = BuildCST(getTokens());
  if (!CST) {
    getEM().increaseErrorCount();
    return nullptr;
  }
  return CST;
}

void Driver::runPrintTokens() {
  if (runTokenize())
    return;
  auto OS = getStdOstream();
  if (!OS)
    return;
  PrintTokens(getTokens(), *OS);
}

void Driver::runAssembleMips() { runAssemble(); }

void Driver::runPrintByteCode() {
  if (runAnalyses())
    return;
  auto OS = getStdOstream();
  if (!OS)
    return;
  PrintByteCode(getProgram(), *OS);
}

void Driver::runPrintAST() {
  if (runAnalyses())
    return;
  auto OS = getStdOstream();
  if (!OS)
    return;
  PrettyPrintAST(*getProgram(), *OS);
}

void Driver::runCheckOnly() { runAnalyses(); }

void Driver::runPrintCST() {
  auto ParseTree = runBuildCST();
  if (!ParseTree) {
    return;
  }
  auto OS = getStdOstream();
  if (!OS)
    return;
  Print(*OS, *ParseTree);
}

void Driver::runPrintByteCodeModule() {
  if (runCodeGen())
    return;
  auto OS = getStdOstream();
  if (!OS)
    return;
  Print(*OS, getByteCodeModule());
}

void Driver::runDumpSymbolTable() {
  if (runAnalyses())
    return;
  auto OS = getStdOstream();
  if (!OS)
    return;
  Print(*OS, getSymbolTable());
}

void Driver::runTransform() {
  if (DriverBase::runTransform()) {
    return;
  }
  auto OS = getStdOstream();
  if (!OS)
    return;
  PrettyPrintAST(*getProgram(), *OS);
}

int Driver::run(int argc, char **argv) {
  namespace tclap = TCLAP;
  tclap::CmdLine Parser("Simple Compiler", ' ', "2.0");
  std::vector<tclap::Arg *> Switches;
  tclap::UnlabeledValueArg<std::string> InputArg(
      "input", "input file (default to stdin)", false, "", "input-file");
  tclap::ValueArg<std::string> OutputArg("o", "output",
                                         "output file (default to stdout)",
                                         false, "", "output-file");

#define HANDLE_COMMAND(Name, Arg, Description)                                 \
  tclap::SwitchArg Name##Switch("", Arg, Description, false);                  \
  Switches.push_back(&Name##Switch);
#include "simplecc/Driver/Driver.def"
  Parser.xorAdd(Switches);

  try {
    Parser.parse(argc, argv);
  } catch (tclap::ArgException &Exc) {
    PrintErrs(Exc.error(), "at argument", Exc.argId());
    return 1;
  }
  setInputFile(InputArg.isSet() ? InputArg.getValue() : "-");
  setOutputFile(OutputArg.isSet() ? OutputArg.getValue() : "-");
#define HANDLE_COMMAND(Name, Arg, Description)                                 \
  if (Name##Switch.isSet()) {                                                  \
    run##Name();                                                               \
    return status();                                                           \
  }
#include "simplecc/Driver/Driver.def"
  assert(false && "Unhandled command line switch!");
}