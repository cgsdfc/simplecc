#include "CompilerInstance.h"

#include <fstream>
#include <iostream>

using namespace simplecompiler;

int main(int argc, char **argv) {
  ErrorManager e;
  Print(std::clog, "Please input a filename:");
  String filename;
  if (!std::getline(std::cin, filename)) {
    e.Error("No filename provided");
    return 1;
  }

  std::ifstream input_file(filename);
  if (input_file.fail()) {
    e.FileReadError(filename);
    return 1;
  }

  CompilerInstance instance(input_file, std::cout);
  bool result = instance.Invoke();
  Print(std::clog, "Press any key to continue");
  std::getchar();
  return !result;
}
