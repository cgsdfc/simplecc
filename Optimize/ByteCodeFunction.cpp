#include "ByteCodeFunction.h"
#include "ByteCodeModule.h"

#include <utility> // pair
#include <algorithm>
#include <iomanip>
#include <iostream>

using namespace simplecompiler;

ByteCodeFunction::ByteCodeFunction(ByteCodeModule *M) : Parent(M) {
  /// Owned by Module
  M->getFunctionList().push_back(this);
}

ByteCodeModule::~ByteCodeModule() {
  /// Delete all owned functions.
  std::for_each(begin(), end(), [](ByteCodeFunction *F) { delete F; });
}

unsigned ByteCodeModule::GetStringLiteralID(const String &Str) {
  auto ID = StringLiterals.size();
  return StringLiterals.emplace(Str, ID).first->second;
}

void ByteCodeModule::clear() {
  FunctionList.clear();
  StringLiterals.clear();
  GlobalVariables.clear();
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

void ByteCodeModule::Format(std::ostream &O) const {
  for (const SymbolEntry &GV : GetGlobalVariables()) {
    O << GV << "\n";
  }

  O << "\n";
  for (const std::pair<String, unsigned> &Pair : GetStringLiteralTable()) {
    O << std::setw(4) << Pair.second << ": " << Pair.first << "\n";
  }

  O << "\n";
  for (const ByteCodeFunction *Fn : *this) {
    O << *Fn << "\n";
  }
}
