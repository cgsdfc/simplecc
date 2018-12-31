
#include <simplecc/Driver/Driver.h>

#include "simplecc/Driver/Driver.h"
#include "simplecc/CodeGen/Compile.h"
#include "simplecc/Lex/Tokenize.h"
#include "simplecc/Target/Assemble.h"
#include "simplecc/Transform/Transform.h"

#ifdef SIMPLE_COMPILER_USE_LLVM
#include "simplecc/LLVM/EmitLLVM.h"
#include "simplecc/Visualize/Visualize.h"
#endif

using namespace simplecc;

void Driver::clear() {
  InputFile.clear();
  OutputFile.clear();
  StdIFStream.clear();
  StdOFStream.clear();
  TheTokens.clear();
  AM.clear();
  TheProgram.release();
  TheModule.clear();
  EM.clear();
}

std::ostream *Driver::getStdOstream() {
  if (OutputFile == "-")
    return &std::cout;
  StdOFStream.open(OutputFile);
  if (StdOFStream.fail()) {
    EM.setErrorType("FileWriteError");
    EM.Error(Quote(OutputFile));
    return nullptr;
  }
  return &StdOFStream;
}

std::istream *Driver::getStdIstream() {
  if (InputFile == "-")
    return &std::cin;
  StdIFStream.open(InputFile);
  if (StdIFStream.fail()) {
    EM.setErrorType("FileReadError");
    EM.Error(Quote(InputFile));
    return nullptr;
  }
  return &StdIFStream;
}

bool Driver::doTokenize() {
  auto Istream = getStdIstream();
  if (!Istream)
    return true;
  // Tokenize never fails.
  Tokenize(*Istream, TheTokens);
  return false;
}

bool Driver::doParse() {
  if (doTokenize())
    return true;
  TheProgram = BuildAST(getInputFile(), TheTokens);
  if (TheProgram)
    return false;
  EM.increaseErrorCount();
  return true;
}

bool Driver::doAnalyses() {
  if (doParse())
    return true;
  assert(TheProgram && "Null Program!");
  if (AM.runAllAnalyses(TheProgram.get())) {
    EM.increaseErrorCount();
    return true;
  }
  return false;
}

bool Driver::doCodeGen() {
  if (doAnalyses())
    return true;
  // CodeGen never fails.
  CompileToByteCode(TheProgram.get(), AM.getSymbolTable(), TheModule);
  return false;
}

bool Driver::doAssemble() {
  auto OS = getStdOstream();
  if (!OS)
    return false;
  if (doCodeGen())
    return true;
  AssembleMips(TheModule, *OS);
  return false;
}

std::unique_ptr<Node> Driver::doBuildCST() {
  if (doTokenize())
    return nullptr;
  std::unique_ptr<Node> TheCST = BuildCST(TheTokens);
  if (!TheCST) {
    EM.increaseErrorCount();
  }
  return TheCST;
}

bool Driver::doTransform() {
  if (doAnalyses())
    return true;
  TransformProgram(TheProgram.get(), AM.getSymbolTable());
  return false;
}

void Driver::runPrintTokens() {
  auto OS = getStdOstream();
  if (!OS)
    return;
  if (doTokenize())
    return;
  PrintTokens(TheTokens, *OS);
}

void Driver::runAssembleMips() { doAssemble(); }

void Driver::runPrintByteCode() {
  if (doAnalyses())
    return;
  auto OS = getStdOstream();
  if (!OS)
    return;
  PrintByteCode(TheProgram.get(), *OS);
}

void Driver::runPrintAST() {
  if (doAnalyses())
    return;
  auto OS = getStdOstream();
  if (!OS)
    return;
  PrettyPrintAST(*TheProgram, *OS);
}

void Driver::runCheckOnly() { doAnalyses(); }

void Driver::runPrintCST() {
  auto TheCST = doBuildCST();
  if (!TheCST)
    return;
  auto OS = getStdOstream();
  if (!OS)
    return;
  Print(*OS, *TheCST);
}

void Driver::runPrintByteCodeModule() {
  if (doCodeGen())
    return;
  auto OS = getStdOstream();
  if (!OS)
    return;
  Print(*OS, TheModule);
}

void Driver::runDumpSymbolTable() {
  if (doAnalyses())
    return;
  auto OS = getStdOstream();
  if (!OS)
    return;
  Print(*OS, AM.getSymbolTable());
}

#ifdef SIMPLE_COMPILER_USE_LLVM
void Driver::runEmitLLVMIR() {
  if (doAnalyses())
    return;
  auto OS = getLLVMRawOstream();
  if (!OS)
    return;
  if (CompileToLLVMIR(TheProgram.get(), AM.getSymbolTable(), *OS)) {
    EM.increaseErrorCount();
  }
}

void Driver::runWriteASTGraph() {
  if (doAnalyses())
    return;
  auto OS = getLLVMRawOstream();
  if (!OS)
    return;
  // PrintAllAstNodes(TheProgram.get(), std::cout);
  WriteASTGraph(TheProgram.get(), *OS);
}

void Driver::runWriteCSTGraph() {
  auto TheCST = doBuildCST();
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
    EM.setErrorType("FileWriteError");
    EM.Error(Quote(getOutputFile()));
    return nullptr;
  }
  return std::move(OS);
}

#endif