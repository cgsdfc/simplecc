#include "Pass.h"
#include "error.h"

#include <fstream>
#include <tclap/CmdLine.h>

using namespace TCLAP;
using namespace simplecompiler;


int main(int argc, char **argv) {
  ErrorManager e;
  CmdLine parser("Simple Compiler Debugging helper", ' ', "0.0.1");


  // Positional argument: input
  UnlabeledValueArg<String> input_arg("input", "input file (default to stdin)",
                                      false, "", "input-file", parser);

  ValueArg<String> output_arg("o", "output", "output file (default to stdout)",
                              false, "", "output-file", parser);
#ifdef SIMPLE_COMPILER_USE_LLVM
  ValueArg<String> format_arg("f", "format", "output format", false, "",
                              "format", parser);
#endif
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

}
