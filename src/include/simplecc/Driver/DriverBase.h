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
  const ProgramAST *getProgram() const { return TheProgram.get(); }
  ProgramAST *getProgram() { return TheProgram.get(); }
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
  std::unique_ptr<ProgramAST, DeleteAST> TheProgram;
  ByteCodeModule TheModule;
  ErrorManager EM;
};
}
#endif //SIMPLECC_DRIVER_DRIVERBASE_H