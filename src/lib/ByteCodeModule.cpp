#include "simplecc/ByteCodeModule.h"
#include "simplecc/ByteCodeCompiler.h"
#include "simplecc/ByteCodeFunction.h"

#include <iomanip>
#include <algorithm>

using namespace simplecc;

void ByteCodeModule::Build(Program *P, const SymbolTable &S) {
  clear();
  ByteCodeCompiler().Compile(P, S, *this);
}

void ByteCodeModule::clear() {
  FunctionList.clear();
  StringLiterals.clear();
  GlobalVariables.clear();
}

ByteCodeModule::~ByteCodeModule() {
  /// Delete all owned functions.
  std::for_each(begin(), end(), [](ByteCodeFunction *F) { delete F; });
}

unsigned ByteCodeModule::getStringLiteralID(const String &Str) {
  auto ID = StringLiterals.size();
  return StringLiterals.emplace(Str, ID).first->second;
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