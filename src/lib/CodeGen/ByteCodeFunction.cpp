#include "simplecc/CodeGen/ByteCodeFunction.h"
#include "simplecc/CodeGen/ByteCodeModule.h"

#include <iomanip>

using namespace simplecc;

ByteCodeFunction::ByteCodeFunction(ByteCodeModule *M) : Parent(M) {
  /// Owned by Module
  M->getFunctionList().push_back(this);
}

void ByteCodeFunction::Format(std::ostream &O) const {
  O << getName() << ":\n";

  for (const SymbolEntry &Arg : getFormalArguments()) {
    O << Arg << "\n";
  }

  for (const SymbolEntry &LV : getLocalVariables()) {
    O << LV << "\n";
  }

  for (const ByteCode &Code : *this) {
    O << Code << "\n";
  }
}