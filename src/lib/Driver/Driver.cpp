#include "simplecc/Driver/Driver.h"
#include "simplecc/CodeGen/Compile.h"
#include "simplecc/Lex/Tokenize.h"
#include "simplecc/Parse/Parse.h"
#include "simplecc/Target/Assemble.h"

#ifdef SIMPLE_COMPILER_USE_LLVM
#include "simplecc/LLVM/EmitLLVM.h"
#include "simplecc/Visualize/Visualize.h"
#include <llvm/Support/raw_ostream.h>
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
  TheProgram = BuildAST(TheTokens);
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
  auto OStream = getStdOstream();
  if (!OStream)
    return false;
  if (doCodeGen())
    return true;
  AssembleMips(TheModule, *OStream);
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

void Driver::runPrintTokens() {
  auto Ostream = getStdOstream();
  if (!Ostream)
    return;
  if (doTokenize())
    return;
  PrintTokens(TheTokens, *Ostream);
}

void Driver::runAssembleMips() { doAssemble(); }

void Driver::runPrintByteCode() {
  if (doAnalyses())
    return;
  auto OStream = getStdOstream();
  if (!OStream)
    return;
  PrintByteCode(TheProgram.get(), *OStream);
}

void Driver::runDumpAst() {
  if (doAnalyses())
    return;
  auto OStream = getStdOstream();
  if (!OStream)
    return;
  /// TODO: use an AstPrettyPrinter.
  Print(*OStream, *TheProgram);
}

void Driver::runAnalysisOnly() { doAnalyses(); }

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
  bool Result = CompileToLLVMIR(getInputFile(), TheProgram.get(),
                                AM.getSymbolTable(), *OS);
  if (!Result) {
    EM.increaseErrorCount();
  }
}

void Driver::runWriteAstGraph() {
  if (doAnalyses())
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