#include "simplecc/Driver/Driver.h"
#include "simplecc/Target/Assemble.h"
#include "simplecc/Lex/Tokenize.h"
#include "simplecc/Parse/Parse.h"
#include "simplecc/Codegen/Compile.h"

#ifdef SIMPLE_COMPILER_USE_LLVM
#include "simplecc/Visualize/Visualize.h"
#include "simplecc/LLVM/EmitLLVM.h"
#endif

#include <system_error>
#include <vector>
#include <simplecc/Driver/Driver.h>

using namespace simplecc;

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
    return false;
  // Tokenize never fails.
  Tokenize(*Istream, TheTokens);
  return true;
}

bool Driver::doParse() {
  if (!doTokenize())
    return false;
  TheProgram = BuildAST(TheTokens);
  if (TheProgram)
    return true;
  EM.increaseErrorCount();
  return false;
}

bool Driver::doAnalyses() {
  if (!doParse())
    return false;
  assert(TheProgram && "Null Program!");
  bool Result = AM.runAllAnalyses(TheProgram.get());
  if (Result)
    return true;
  EM.increaseErrorCount();
  return false;
}

bool Driver::doCodeGen() {
  if (!doAnalyses())
    return false;
  // Codegen never fails.
  CompileToByteCode(TheProgram.get(), AM.getSymbolTable(), TheModule);
  return true;
}

bool Driver::doAssemble() {
  auto OStream = getStdOstream();
  if (!OStream)
    return false;
  if (!doCodeGen())
    return false;
  AssembleMips(TheModule, *OStream);
  return true;
}

std::unique_ptr<Node> Driver::doBuildCST() {
  if (!doTokenize())
    return nullptr;
  std::unique_ptr<Node> TheCST = BuildCST(TheTokens);
  if (!TheCST) {
    EM.increaseErrorCount();
  }
  return TheCST;
}

void Driver::runPrintTokens() {
  auto Ostream = getStdOstream();
  if (!Ostream)
    return;
  if (!doTokenize())
    return;
  PrintTokens(TheTokens, *Ostream);
}

void Driver::runAssembleMips() {
  doAssemble();
}

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

void Driver::runPrintByteCode() {
  if (!doAnalyses())
    return;
  auto OStream = getStdOstream();
  if (!OStream)
    return;
  PrintByteCode(TheProgram.get(), *OStream);
}

void Driver::runDumpAst() {
  if (!doAnalyses())
    return;
  auto OStream = getStdOstream();
  if (!OStream)
    return;
  /// TODO: use an AstPrettyPrinter.
  Print(*OStream, *TheProgram);
}

void Driver::runAnalysisOnly() {
  doAnalyses();
}

void Driver::runDumpCst() {
  auto TheCST = doBuildCST();
  if (!TheCST)
    return;
  auto OS = getStdOstream();
  if (!OS)
    return;
  Print(*OS, *TheCST);
}

void Driver::runDumpByteCodeModule() {
  if (!doCodeGen())
    return;
  auto OS = getStdOstream();
  if (!OS)
    return;
  Print(*OS, TheModule);
}

#ifdef SIMPLE_COMPILER_USE_LLVM

void Driver::runEmitLLVMIR() {
  if (!doAnalyses())
    return;
  bool Result = CompileToLLVMIR(getInputFile(), TheProgram.get(),
                                AM.getSymbolTable(), getOutputFile());
  if (!Result) {
    EM.increaseErrorCount();
  }
}

void Driver::runWriteAstGraph() {
  if (!doAnalyses())
    return;
  auto OS = getLLVMRawOstream();
  if (!OS)
    return;
  WriteASTGraph(TheProgram.get(), *OS);
}

void Driver::runWriteCstGraph() {
  auto TheCST = doBuildCST();
  if (!TheCST)
    return;
  auto OS = getLLVMRawOstream();
  if (!OS)
    return;
  WriteCSTGraph(TheCST.get(), *OS);
}

llvm::raw_ostream *Driver::getLLVMRawOstream() {
  std::error_code EC;
  LLVMRawFdOstream.reset(new llvm::raw_fd_ostream(InputFile, EC));
  if (EC) {
    // Destroy the raw_fd_ostream as told by their doc.
    LLVMRawFdOstream.release();
    EM.setErrorType("FileWriteError");
    EM.Error(Quote(getInputFile()));
    return nullptr;
  }
  return LLVMRawFdOstream.get();
}

#endif