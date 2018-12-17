#include "simplecompiler/ErrorManager.h"
#include "simplecompiler/Pass.h"
#include "simplecompiler/Print.h"

#include <cassert>
#include <cstdio>   // for getchar()
#include <iostream> // for cin

using namespace simplecompiler;

int main(int argc, char **argv) {
  /// Prompt the user for an input filename.
  PrintErrs("Please input a filename:");
  String Filename;
  ErrorManager EM;

  /// If the user hit Control-D or Control-Z + Enter.
  if (!std::getline(std::cin, Filename)) {
    EM.Error("No filename provided");
    return 1;
  }

  PassManager PM;
  /// If the supplied filename isn't readable.
  if (!PM.setInputFile(Filename)) {
    EM.FileReadError(Filename);
    return 1;
  }

  /// Get the ID of the AssemblePass.
  PassInfo *PI = getGlobalRegistry().getPassInfo("assemble");
  assert(PI && "The AssemblePass must be registered");

  /// Run the AssemblePass with the PM.
  bool OK = PM.run(PI->getID());

  /// To be Windows-friendly.
  PrintErrs("Press any key to continue");
  std::getchar();
  return !OK;
}
