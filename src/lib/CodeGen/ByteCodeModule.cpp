// MIT License
// 
// Copyright (c) 2018 Cong Feng.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "simplecc/CodeGen/ByteCodeModule.h"
#include "simplecc/CodeGen/ByteCodeCompiler.h"
#include "simplecc/CodeGen/ByteCodeFunction.h"
#include <algorithm> // for_each
#include <iomanip> // setw
#include <memory> // default_delete

using namespace simplecc;

void ByteCodeModule::clear() {
  // Delete the function first.
  std::for_each(begin(), end(), std::default_delete<ByteCodeFunction>());
  FunctionList.clear();
  StringLiterals.clear();
  GlobalVariables.clear();
}

ByteCodeModule::~ByteCodeModule() {
  /// Delete all owned functions.
  std::for_each(begin(), end(), std::default_delete<ByteCodeFunction>());
}

unsigned ByteCodeModule::getStringLiteralID(const std::string &Str) {
  auto ID = static_cast<unsigned int>(StringLiterals.size());
  return StringLiterals.emplace(Str, ID).first->second;
}

// TODO: make the printing better (currently like a shut).
void ByteCodeModule::Format(std::ostream &O) const {
  for (const SymbolEntry &GV : getGlobalVariables()) {
    O << GV << "\n";
  }

  O << "\n";
  for (const auto &Pair : getStringLiteralTable()) {
    O << std::setw(4) << Pair.second << ": " << Pair.first << "\n";
  }

  O << "\n";
  for (const ByteCodeFunction *Fn : *this) {
    O << *Fn << "\n";
  }
}