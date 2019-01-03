#ifndef SIMPLECC_DRIVER_DRIVERBASE_H
#define SIMPLECC_DRIVER_DRIVERBASE_H
#include "simplecc/Analysis/AnalysisManager.h"
#include "simplecc/CodeGen/ByteCodeModule.h"
#include "simplecc/Lex/TokenInfo.h"
#include "simplecc/Support/ErrorManager.h"
#include "simplecc/Parse/Parse.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

namespace simplecc {
class DriverBase {
protected:
  /// Return a ptr to the output stream. Nullptr on failure.
  std::ostream *getStdOstream();
  /// Return a ptr to the input stream. Nullptr on failure.
  std::istream *getStdIstream();

  /// Lower level interfaces, each of which wraps a component function.
  void doTokenize(std::istream &IS);
  bool doParse();
  bool doAnalyses();
  void doTransform();
  void doCodeGen();
  void doAssemble(std::ostream &OS);

  /// High level interfaces, each of which run all its dependencies and
  /// can be run individually.
  bool runTokenize();
  bool runParse();
  bool runAnalyses();
  bool runTransform();
  bool runCodeGen();
  bool runAssemble();

  const std::vector<TokenInfo> &getTokens() const { return TheTokens; }
  const SymbolTable &getSymbolTable() const { return AM.getSymbolTable(); }
  const Program *getProgram() const { return TheProgram.get(); }
  Program *getProgram() { return TheProgram.get(); }
  const ByteCodeModule &getByteCodeModule() const { return TheModule; }
  ByteCodeModule &getByteCodeModule() { return TheModule; }
  ErrorManager &getEM() { return EM; }
public:
  void setInputFile(std::string Filename) { InputFile = std::move(Filename); }
  void setOutputFile(std::string Filename) { OutputFile = std::move(Filename); }
  std::string getInputFile() const { return InputFile; }
  std::string getOutputFile() const { return OutputFile; }

  void clear();
  int status() const { return !EM.IsOk(); }

private:
  std::string InputFile;
  std::string OutputFile;
  std::ifstream StdIFStream;
  std::ofstream StdOFStream;

  std::vector<TokenInfo> TheTokens;
  AnalysisManager AM;
  ProgramRef TheProgram;
  ByteCodeModule TheModule;
  ErrorManager EM;
};
}
#endif //SIMPLECC_DRIVER_DRIVERBASE_H