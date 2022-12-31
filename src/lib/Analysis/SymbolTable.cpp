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

#include "simplecc/Analysis/SymbolTable.h"

using namespace simplecc;

void SymbolTable::Format(std::ostream &O) const {
  O << "Global:\n";
  for (const auto &Pair : GlobalTable) {
    O << "  " << Pair.first << ": " << Pair.second << "\n";
  }
  O << "\n";
  for (const auto &Pair : LocalTables) {
    O << "Local(" << Pair.first->getName() << "):\n";
    for (const auto &Item : Pair.second) {
      O << "  " << Item.first << ": " << Item.second << "\n";
    }
    O << "\n";
  }
}

void SymbolTable::clear() {
  GlobalTable.clear();
  LocalTables.clear();
}

LocalSymbolTable SymbolTable::getLocalTable(const FuncDef *FD) const {
  assert(LocalTables.count(FD));
  return LocalSymbolTable(LocalTables.find(FD)->second);
}

SymbolEntry SymbolTable::getGlobalEntry(const std::string &Name) const {
  assert(GlobalTable.count(Name));
  return GlobalTable.find(Name)->second;
}

SymbolEntry LocalSymbolTable::operator[](const std::string &Name) const {
  assert(TheTable->count(Name) && "Undefined Name");
  return TheTable->find(Name)->second;
}