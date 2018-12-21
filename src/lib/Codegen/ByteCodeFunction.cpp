#include "simplecc/Codegen/ByteCodeFunction.h"
#include "simplecc/Codegen/ByteCodeModule.h"

#include <iomanip>

using namespace simplecc;

ByteCodeFunction::ByteCodeFunction(ByteCodeModule *M) : Parent(M) {
  /// Owned by Module
  M->getFunctionList().push_back(this);
}

void ByteCodeFunction::Format(std::ostream &O) const {
  O << getName() << ":\n";

  for (const SymbolEntry &Arg : GetFormalArguments()) {
    O << Arg << "\n";
  }

  for (const SymbolEntry &LV : GetLocalVariables()) {
    O << LV << "\n";
  }

  for (const ByteCode &Code : *this) {
    O << Code << "\n";
  }
}