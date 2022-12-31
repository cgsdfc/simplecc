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

#include "simplecc/Driver/DriverBase.h"
#include "simplecc/Lex/Tokenize.h"
#include "simplecc/CodeGen/CodeGen.h"
#include "simplecc/Target/Target.h"
#include "simplecc/Transform/Transform.h"

using namespace simplecc;

std::ostream *DriverBase::getStdOstream() {
  if (OutputFile == "-")
    return &std::cout;
  StdOFStream.open(OutputFile);
  if (StdOFStream.fail()) {
    EM.setErrorType("FileWriteError");
    EM.Error(OutputFile);
    return nullptr;
  }
  return &StdOFStream;
}

std::istream *DriverBase::getStdIstream() {
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

void DriverBase::doTokenize(std::istream &IS) {
  Tokenize(IS, TheTokens);
}

bool DriverBase::doParse() {
  TheProgram = BuildAST(getInputFile(), TheTokens);
  return !TheProgram;
}

bool DriverBase::doAnalyses() {
  return AM.runAllAnalyses(TheProgram.get());
}

void DriverBase::doCodeGen() {
  CompileToByteCode(TheProgram.get(), AM.getSymbolTable(), TheModule);
}

void DriverBase::doAssemble(std::ostream &OS) {
  AssembleMips(TheModule, OS);
}

void DriverBase::doTransform() {
  TransformProgram(TheProgram.get(), AM.getSymbolTable());
}

bool DriverBase::runTokenize() {
  auto IS = getStdIstream();
  if (!IS)
    return true;
  doTokenize(*IS);
  return false;
}

bool DriverBase::runParse() {
  if (runTokenize())
    return true;
  if (doParse()) {
    EM.increaseErrorCount();
    return true;
  }
  return false;
}

bool DriverBase::runAnalyses() {
  if (runParse())
    return true;
  if (doAnalyses()) {
    EM.increaseErrorCount();
    return true;
  }
  return false;
}

bool DriverBase::runTransform() {
  if (runAnalyses())
    return true;
  doTransform();
  return false;
}

bool DriverBase::runCodeGen() {
  if (runTransform())
    return true;
  doCodeGen();
  return false;
}

bool DriverBase::runAssemble() {
  auto OS = getStdOstream();
  if (!OS)
    return true;
  if (runCodeGen())
    return true;
  doAssemble(*OS);
  return false;
}

void DriverBase::clear() {
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