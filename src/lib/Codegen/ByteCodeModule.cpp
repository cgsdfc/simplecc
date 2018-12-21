#include "simplecc/Codegen/ByteCodeModule.h"
#include "simplecc/Codegen/ByteCodeCompiler.h"
#include "simplecc/Codegen/ByteCodeFunction.h"

#include <iomanip>
#include <algorithm>

using namespace simplecc;

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
  auto ID = static_cast<unsigned int>(StringLiterals.size());
  return StringLiterals.emplace(Str, ID).first->second;
}

void ByteCodeModule::Format(std::ostream &O) const {
  for (const SymbolEntry &GV : getGlobalVariables()) {
    O << GV << "\n";
  }

  O << "\n";
  for (const std::pair<const String, unsigned> &Pair : getStringLiteralTable()) {
    O << std::setw(4) << Pair.second << ": " << Pair.first << "\n";
  }

  O << "\n";
  for (const ByteCodeFunction *Fn : *this) {
    O << *Fn << "\n";
  }
}