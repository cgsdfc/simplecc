#include "CompilerInstance.h"
#include "error.h"

#include <fstream>
#include <tclap/CmdLine.h>

using namespace TCLAP;
using namespace simplecompiler;

using ArgToCompilationPhraseTable =
    std::unordered_map<TCLAP::Arg *, CompilationPhrase>;

// Find the selected argument -- isSet()'ed
CompilationPhrase FindSelectedPhrase(const ArgToCompilationPhraseTable &map) {
  auto iter =
      std::find_if(map.begin(), map.end(),
                   [](ArgToCompilationPhraseTable::const_reference item) {
                     return item.first->isSet();
                   });
  assert(iter != map.end());
  return iter->second;
}

// For debugging
inline std::ostream &operator<<(std::ostream &os, CompilationPhrase phrase) {
  return os << static_cast<int>(phrase);
}

int main(int argc, char **argv) {
  ErrorManager e;
  CmdLine parser("Simple Compiler Debugging helper", ' ', "0.0.1");

  SwitchArg tokenize("", "tokenize", "break input into tokens", false);
  SwitchArg build_cst("", "build-cst",
                      "run the parser and construct a concrete syntax tree",
                      false);
  SwitchArg build_ast(
      "", "build-ast",
      "create an abstract syntax tree from the concrete syntax tree", false);
  SwitchArg syntax_check("", "syntax-check",
                         "verify that the AST is syntactically correct", false);
  SwitchArg build_symtable("", "build-symtable",
                           "build a symbol table from the abstract syntax tree",
                           false);
  SwitchArg type_check("", "type-check", "run type-check for the input program",
                       false);
  SwitchArg print_bytecode("", "print-bytecode",
                           "print byte code in quarternary form", false);
  SwitchArg compile("", "compile", "compile the input program to byte code",
                    false);
  SwitchArg assemble("", "assemble",
                     "assemble the compiled program to MIPS assembly", false);

  // XXX: If any option is added to the list above, you **must** add an entry
  // here!
  std::vector<TCLAP::Arg *> xor_list{
      &tokenize,   &build_cst,      &build_ast, &syntax_check, &build_symtable,
      &type_check, &print_bytecode, &compile,   &assemble,
  };

  parser.xorAdd(xor_list);

  // Hard-coded mapping from Arg to CompilationPhrase
  ArgToCompilationPhraseTable args_phrase_map{
      {&tokenize, CompilationPhrase::Tokenize},
      {&build_cst, CompilationPhrase::BuildCst},
      {&build_ast, CompilationPhrase::BuildAst},
      {&syntax_check, CompilationPhrase::SyntaxCheck},
      {&build_symtable, CompilationPhrase::BuildSymbolTable},
      {&type_check, CompilationPhrase::TypeCheck},
      {&print_bytecode, CompilationPhrase::PrintByteCode},
      {&compile, CompilationPhrase::Compile},
      {&assemble, CompilationPhrase::Assemble},
  };

  assert(xor_list.size() == args_phrase_map.size());

  // Positional argument: input
  UnlabeledValueArg<String> input_arg("input", "input file (default to stdin)",
                                      false, "", "input-file", parser);

  ValueArg<String> output_arg("o", "output", "output file (default to stdout)",
                              false, "", "output-file", parser);

  try {
    parser.parse(argc, argv);
  } catch (TCLAP::ArgException &exception) {
    e.Error(exception.error(), "at argument", exception.argId());
    return 1;
  }

  // Prepare input stream
  std::istream *input_stream;
  std::ifstream input_file;

  if (!input_arg.isSet()) {
    input_stream = &std::cin;
  } else {
    input_file.open(input_arg.getValue());
    if (input_file.fail()) {
      e.FileReadError(input_arg.getValue());
      return 1;
    }
    input_stream = &input_file;
  }

  // Prepare output stream
  std::ostream *output_stream;
  std::ofstream output_file;

  if (!output_arg.isSet()) {
    output_stream = &std::cout;
  } else {
    output_file.open(output_arg.getValue());
    if (output_file.fail()) {
      e.FileWriteError(output_arg.getValue());
      return 1;
    }
    output_stream = &output_file;
  }

  // Determine the phrase to run
  auto phrase = FindSelectedPhrase(args_phrase_map);
  auto instance =
      std::make_unique<CompilerInstance>(*input_stream, *output_stream, phrase);
  return !instance->Invoke();
}
