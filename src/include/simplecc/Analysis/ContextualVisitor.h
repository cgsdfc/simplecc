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

#ifndef SIMPLECC_ANALYSIS_CONTEXTUALVISITOR_H
#define SIMPLECC_ANALYSIS_CONTEXTUALVISITOR_H
#include "simplecc/Analysis/SymbolTable.h"
#include "simplecc/AST/ChildrenVisitor.h"

namespace simplecc {
/// @brief ContextualVisitor is a CRTP base for Visitors that knows the LocalSymbolTable of
/// the FuncDef it is visiting. It sets up the corresponding LocalSymbolTable
/// for each FuncDef to be visited.
template <typename Derived>
class ContextualVisitor : public ChildrenVisitor<Derived> {

protected:
  ContextualVisitor() = default;

  /// Set the SymbolTable.
  void setTable(SymbolTable &S) { TheTable = &S; }
  /// Set the local table.
  void setLocalTable(FuncDef *FD) {
    TheLocalTable = getSymbolTable().getLocalTable(FD);
  }
  /// Return the symbol table.
  const SymbolTable &getSymbolTable() const {
    assert(TheTable);
    return *TheTable;
  }

  SymbolTable &getSymbolTable() {
    assert(TheTable);
    return *TheTable;
  }

  /// Return the SymbolEntry for an name.
  SymbolEntry getSymbolEntry(const std::string &Name) const {
    return TheLocalTable[Name];
  }

public:
  /// Initialize LocalSymbolTable before performing real visit.
  /// Subclass can override it and add more initialization.
  void visitFuncDef(FuncDef *FD) {
    setLocalTable(FD);
    ChildrenVisitor<Derived>::visitFuncDef(FD);
  }

  /// Set the symbol table and then visit the program.
  void visitProgram(ProgramAST *P, SymbolTable &S) {
    setTable(S);
    ChildrenVisitor<Derived>::visitProgram(P);
  }

private:
  SymbolTable *TheTable;
  LocalSymbolTable TheLocalTable;
};
} // namespace simplecc

#endif // SIMPLECC_ANALYSIS_CONTEXTUALVISITOR_H