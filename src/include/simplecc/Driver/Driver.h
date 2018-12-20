#ifndef SIMPLECOMPILER_DRIVER_H
#define SIMPLECOMPILER_DRIVER_H
#include "simplecc/Support/ErrorManager.h"
#include "simplecc/Lex/TokenInfo.h"
#include "simplecc/Analysis/AnalysisManager.h"
#include "simplecc/Codegen/ByteCodeModule.h"

#include <string>
#include <utility>
#include <iostream>
#include <fstream>

#ifdef SIMPLE_COMPILER_USE_LLVM
#include <llvm/Support/raw_ostream.h>
#endif

namespace simplecc {

/// This class exposes a high-level interface to the system in terms of InputFile and OutputFile.
/// It exposes different parts of the system through member functions. It provides both
/// do-all style methods and low level functions such as tokenizing a file.
/// Driver D;
/// D.setInputFile("aaa");
/// D.runAssembleMips();
/// return D.status();
///
class Driver {
  std::ostream *getStdOstream();
  std::istream *getStdIstream();

  bool doTokenize();
  bool doParse();
  bool doAnalyses();
  bool doCodeGen();
  bool doAssemble();

public:
  Driver() = default;

  void setInputFile(std::string Filename) { InputFile = std::move(Filename);}
  void setOutputFile(std::string Filename) { OutputFile = std::move(Filename); }

  std::string getInputFile() const { return InputFile; }
  std::string getOutputFile() const { return OutputFile; }

  void runAssembleMips();
  void runPrintTokens();
  void runPrintByteCode();
  void runDumpByteCodeModule();
  void runAnalysisOnly();
  void runDumpSymbolTable();
  void runDumpAst();

  void runDumpCst();
#ifdef SIMPLE_COMPILER_USE_LLVM
  bool runEmitLLVMIR();
  bool runWriteAstGraph();
  bool runWriteCstGraph();
#endif

  int status() const { return !EM.IsOk(); }

  void clear();

private:
  std::string InputFile;
  std::string OutputFile;

  std::ifstream StdIFStream;
  std::ofstream StdOFStream;

  std::vector<TokenInfo> TheTokens;
  AnalysisManager AM;
  std::unique_ptr<Program> TheProgram;
  ByteCodeModule TheModule;
  ErrorManager EM;
};

}
#endif //SIMPLECOMPILER_DRIVER_H
