#ifndef COMPILER_INSTANCE
#define COMPILER_INSTANCE

#include "SymbolTable.h"
#include "error.h"

#include <iostream>
#include <vector>

namespace simplecompiler {
class Node;
enum class CompilationPhrase {
  Tokenize,
  BuildCst,
  BuildAst,
  SyntaxCheck,
  BuildSymbolTable,
  TypeCheck,
  PrintByteCode,
  Compile,
  Assemble,
  EmitLLVM,
};

enum class OutputFormat {
  RawDump,
#ifdef SIMPLE_COMPILER_USE_LLVM
  DOT,
#endif
};

class CompilerOptions {
  CompilationPhrase P;
  OutputFormat F;

public:
  CompilerOptions()
      : P(CompilationPhrase::Assemble), F(OutputFormat::RawDump) {}
  CompilerOptions(CompilationPhrase P, OutputFormat F) : P(P), F(F) {}
  CompilerOptions(const CompilerOptions &) = default;
  CompilationPhrase getPhrase() const { return P; }
  OutputFormat getOutputFormat() const { return F; }
  void setPhrase(CompilationPhrase Phrase) { P = Phrase; }
  void setFormat(OutputFormat Format) { F = Format; }
};

class CompilerInstance {
  std::istream &input;
  std::ostream &output;
  CompilerOptions Options;
  std::vector<TokenInfo> tokens;
  SymbolTable symbolTable;
  Node *cst_node;
  Program *ast_node;

public:
  CompilerInstance(std::istream &input, std::ostream &output);
  ~CompilerInstance();

  CompilerInstance(const CompilerInstance &) = delete;
  CompilerInstance(CompilerInstance &&) = delete;

  bool Invoke();
  void setOptions(const CompilerOptions &O) { Options = O; }
};
} // namespace simplecompiler
#endif
