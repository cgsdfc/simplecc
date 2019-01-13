#include "simplecc/Driver/DriverBase.h"
#include "simplecc/Lex/Tokenize.h"
#include "simplecc/CodeGen/Compile.h"
#include "simplecc/Target/Assemble.h"
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