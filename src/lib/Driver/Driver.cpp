// MIT License
// 
// Copyright (c) 2018 Cong Feng.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "simplecc/Driver/Driver.h"
#include "simplecc/CodeGen/CodeGen.h"
#include "simplecc/Lex/Tokenize.h"
#include "simplecc/Target/Target.h"
#include "simplecc/Transform/Transform.h"
#include <tclap/CmdLine.h>

#if SIMPLE_COMPILER_USE_LLVM
#include "simplecc/LLVM/LLVM.h"
#include "simplecc/Visualize/Visualize.h"
#endif

using namespace simplecc;

#if SIMPLE_COMPILER_USE_LLVM
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
  tclap::CmdLine Parser("A simple yet modular C-like compiler", ' ', "3.0");
  std::vector<tclap::Arg *> Switches;
  tclap::UnlabeledValueArg<std::string> InputArg(
      "input", "input file (default to stdin)", false, "", "input-file", Parser);
  tclap::ValueArg<std::string> OutputArg("o", "output",
                                         "output file (default to stdout)",
                                         false, "", "output-file", Parser);

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