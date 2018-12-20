#include "simplecc/Driver/Driver.h"
#include "simplecc/Target/Assemble.h"
#include "simplecc/Lex/Tokenize.h"
#include "simplecc/Parse/Parse.h"
#include "simplecc/Codegen/ByteCodePrinter.h"
#include "simplecc/Codegen/ByteCodeCompiler.h"

#include <system_error>
#include <vector>

using namespace simplecc;

std::ostream *Driver::getStdOstream() {
  if (OutputFile == "-")
    return &std::cout;
  StdOFStream.open(OutputFile);
  if (StdOFStream.fail()) {
    EM.setErrorType("IOError");
    EM.Error("cannot open", OutputFile, "for writing");
    return nullptr;
  }
  return &StdOFStream;
}

std::istream *Driver::getStdIstream() {
  if (InputFile == "-")
    return &std::cin;
  StdIFStream.open(InputFile);
  if (StdIFStream.fail()) {
    EM.setErrorType("IOError");
    EM.Error("cannot open", InputFile, "for reading");
    return nullptr;
  }
  return &StdIFStream;
}

bool Driver::doTokenize() {
  auto Istream = getStdIstream();
  if (!Istream)
    return false;
  // Tokenize never fails.
  Tokenize(*Istream, Tokens);
  return true;
}

bool Driver::doParse() {
  if (!doTokenize())
    return false;
  TheProgram = BuildAST(Tokens);
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
  ByteCodeCompiler().Compile(TheProgram.get(), AM.getSymbolTable(), TheModule);
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

void Driver::runPrintTokens() {
  auto Ostream = getStdOstream();
  if (!Ostream)
    return;
  if (!doTokenize())
    return;
  PrintTokens(Tokens, *Ostream);
}

void Driver::runAssembleMips() {
  doAssemble();
}



void Driver::clear() {
  InputFile.clear();
  OutputFile.clear();
  StdIFStream.clear();
  StdOFStream.clear();
  Tokens.clear();
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
  if (!doTokenize())
    return;
  std::unique_ptr<Node> N = BuildCST(Tokens);
  if (!N) {
    EM.increaseErrorCount();
    return;
  }
  auto OS = getStdOstream();
  if (!OS)
    return;
  Print(*OS, *N);
}

void Driver::runDumpByteCodeModule() {
  if (!doCodeGen())
    return;
  auto OS = getStdOstream();
  if (!OS)
    return;
  Print(*OS, TheModule);
}
