#include "compile.h"
#include "cst.h"
#include "error.h"
#include "mips.h"
#include "parser.h"
#include "symtable.h"
#include "syntax_check.h"
#include "type_check.h"

#include <tclap/CmdLine.h>
#include <fstream>
#include <memory>

using namespace TCLAP;

void TestSymbolTable(SymbolTable &symtable);


int main(int argc, char **argv) {
  CmdLine parser("Simple Compiler Debugging helper", ' ', "0.0.1");

  ValueArg<String> input_arg("i", "input",
      "input file to compile", // description
      true, // required
      "", // default
      "file", // metavar
      parser);

  SwitchArg tokenize("", "tokenize", "break input into tokens", false);
  SwitchArg build_cst("", "build-cst", "run the parser and construct a concrete syntax tree", false);
  SwitchArg build_ast("", "build-ast", "create an abstract syntax tree from the concrete syntax tree", false);
  SwitchArg syntax_check("", "syntax-check", "verify that the AST is syntax correct", false);
  SwitchArg build_symtable("", "build-symtable", "build a symbol table from the abstract syntax tree", false);
  SwitchArg type_check("", "type-check", "run type-check for the input program", false);
  SwitchArg compile("", "compile", "compile the input program to byte code", false);
  SwitchArg assemble("", "assemble", "assemble the compiled program to MIPS assembly", false);
  std::vector<TCLAP::Arg*> xor_list{
    &tokenize, &build_cst, &build_ast, &syntax_check, &build_symtable, &type_check, &compile, &assemble
  };

  parser.xorAdd(xor_list);

  try {
    parser.parse(argc, argv);
  } catch (TCLAP::ArgException &e) {
    Print(std::cerr, "Error:", e.error());
    return 1;
  }

  // Prepare input stream
  std::ifstream input_file(input_arg.getValue());
  if (input_file.fail()) {
    Print(std::cerr, "Error: File",
        Quote(input_arg.getValue()), "does not exist");
    return 1;
  }

  TokenBuffer tokens;
  Tokenize(input_file, tokens);
  if (tokenize.getValue()) {
    PrintTokens(tokens, std::cout);
    return 0;
  }

  std::unique_ptr<Node> cst_node{ParseTokens(tokens)};
  if (!cst_node) {
    // SyntaxError
    return 1;
  }
  if (build_cst.getValue()) {
    std::cout << *cst_node << "\n";
    return 0;
  }

  std::unique_ptr<Program> ast_node{NodeToAst(cst_node.get())};
  if (!ast_node) {
    // No memory
    return 1;
  }
  if (build_ast.getValue()) {
    std::cout << *ast_node << "\n";
    return 0;
  }

  if (!CheckSyntax(ast_node.get())) {
    return 1;
  }
  if (syntax_check.getValue()) {
    return 0;
  }

  SymbolTable symtable;
  if (!BuildSymbolTable(ast_node.get(), symtable)) {
    return 1;
  }
  symtable.Check();
  if (build_symtable.getValue()) {
    std::cout << symtable << "\n";
    return 0;
  }

  if (!CheckType(ast_node.get(), symtable)) {
    return 1;
  }
  if (type_check.getValue()) {
    // Add expr_types information
    std::cout << symtable << "\n";
    return 0;
  }

  auto &&module = CompileProgram(ast_node.get(), symtable);
  if (compile.getValue()) {
    std::cout << module << "\n";
    return 0;
  }

  if (assemble.getValue()) {
    AssembleMips(module, std::cout);
    return 0;
  }

  return 1;
}
