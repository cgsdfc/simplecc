#include "compile.h"
#include "cst.h"
#include "error.h"
#include "assemble.h"
#include "parser.h"
#include "symtable.h"
#include "syntax_check.h"
#include "type_check.h"
#include "ByteCodePrinter.h"

#include <tclap/CmdLine.h>
#include <fstream>
#include <memory>

using namespace TCLAP;


int main(int argc, char **argv) {
  CmdLine parser("Simple Compiler Debugging helper", ' ', "0.0.1");

  SwitchArg tokenize("", "tokenize", "break input into tokens", false);
  SwitchArg build_cst("", "build-cst", "run the parser and construct a concrete syntax tree", false);
  SwitchArg build_ast("", "build-ast", "create an abstract syntax tree from the concrete syntax tree", false);
  SwitchArg syntax_check("", "syntax-check", "verify that the AST is syntactically correct", false);
  SwitchArg build_symtable("", "build-symtable", "build a symbol table from the abstract syntax tree", false);
  SwitchArg type_check("", "type-check", "run type-check for the input program", false);
  SwitchArg print_bytecode("", "print-bytecode", "print byte code in quarternary form", false);
  SwitchArg compile("", "compile", "compile the input program to byte code", false);
  SwitchArg assemble("", "assemble", "assemble the compiled program to MIPS assembly", false);

  // XXX: If any option is added to the list above, you **must** add an entry here!
  std::vector<TCLAP::Arg*> xor_list{
    &tokenize, &build_cst, &build_ast, &syntax_check,
    &build_symtable, &type_check, &print_bytecode, &compile, &assemble
  };

  parser.xorAdd(xor_list);

  // Positional argument: input
  UnlabeledValueArg<String> input_arg("input",
      "input file (default to stdin)", false, "", "input-file", parser);

  ValueArg<String> output_arg("o", "output",
      "output file (default to stdout)", false, "", "output-file", parser);

  try {
    parser.parse(argc, argv);
  } catch (TCLAP::ArgException &e) {
    Print(std::cerr, "Error:", e.error());
    return 1;
  }

  // Prepare input stream
  std::istream *input_stream;
  std::ifstream  input_file;

  if (!input_arg.isSet()) {
    input_stream = &std::cin;
  }
  else {
    input_file.open(input_arg.getValue());
    if (input_file.fail()) {
      Print(std::cerr, "Error: File",
          Quote(input_arg.getValue()), "does not exist");
      return 1;
    }
    input_stream = &input_file;
  }

  // Prepare output stream
  std::ostream *output_stream;
  std::ofstream output_file;

  if (!output_arg.isSet()) {
    output_stream = &std::cout;
  }
  else {
    output_file.open(output_arg.getValue());
    if (output_file.fail()) {
      Print(std::cerr, "Error: File",
          Quote(output_arg.getValue()), "cannot be written to");
      return 1;
    }
    output_stream = &output_file;
  }


  TokenBuffer tokens;
  Tokenize(*input_stream, tokens);
  if (tokenize.getValue()) {
    PrintTokens(tokens, *output_stream);
    return 0;
  }

  std::unique_ptr<Node> cst_node{ParseTokens(tokens)};
  if (!cst_node) {
    // SyntaxError
    return 1;
  }
  if (build_cst.getValue()) {
    *output_stream << *cst_node << "\n";
    return 0;
  }

  std::unique_ptr<Program> ast_node{NodeToAst(cst_node.get())};
  if (!ast_node) {
    // No memory
    return 1;
  }
  if (build_ast.getValue()) {
    *output_stream << *ast_node << "\n";
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
    *output_stream << symtable << "\n";
    return 0;
  }

  if (!CheckType(ast_node.get(), symtable)) {
    return 1;
  }
  if (type_check.getValue()) {
    // Add expr_types information
    *output_stream << symtable << "\n";
    return 0;
  }

  if (print_bytecode.getValue()) {
    PrintByteCode(ast_node.get(), *output_stream);
    return 0;
  }

  auto &&module = CompileProgram(ast_node.get(), symtable);
  if (compile.getValue()) {
    *output_stream << module << "\n";
    return 0;
  }

  if (assemble.getValue()) {
    AssembleMips(module, *output_stream);
    return 0;
  }

  return 1;
}
