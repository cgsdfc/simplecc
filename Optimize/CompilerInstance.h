#ifndef COMPILER_INSTANCE
#define COMPILER_INSTANCE

#include "error.h"
#include "SymbolTable.h"

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
};

class CompilerInstance {
  std::istream &input;
  std::ostream &output;
  CompilationPhrase phrase;
  TokenBuffer tokens;
  SymbolTable symbolTable;
  Node *cst_node;
  Program *ast_node;

public:
  CompilerInstance(std::istream &input, std::ostream &output,
                   CompilationPhrase phrase);

  ~CompilerInstance();

  CompilerInstance(const CompilerInstance &) = delete;
  CompilerInstance(CompilerInstance &&) = delete;

  bool Invoke();
};
} // namespace simplecompiler
#endif