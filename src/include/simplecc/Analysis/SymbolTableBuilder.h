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

#ifndef SIMPLECC_ANALYSIS_SYMBOLTABLEBUILDER_H
#define SIMPLECC_ANALYSIS_SYMBOLTABLEBUILDER_H
#include "simplecc/Analysis/SymbolTable.h"
#include "simplecc/AST/ChildrenVisitor.h"
#include "simplecc/Support/ErrorManager.h"
#include <string>
// TODO: docs
namespace simplecc {
/// This all-in-one class does what MakeLocal(), MakeGlobal()
/// and LocalResolver do and in a uniform Visitor fashion.
class SymbolTableBuilder : ChildrenVisitor<SymbolTableBuilder> {
  /// Why Inherit from ChildrenVisitor:
  /// We inherit from ChildrenVisitor since SymbolTable does not
  /// concern about expression or statements. All it concerns is
  /// declaration and its use site -- Names. So we let ChildrenVisitor
  /// to automatically recurse into children while we only implement
  /// visitors of interest.

  void DefineLocalDecl(DeclAST *D);
  void DefineGlobalDecl(DeclAST *D);
  void visitDecl(DeclAST *D);

  /// Overloads to visit AstNodes that have names.
  void visitName(NameExpr *N) { ResolveName(N->getName(), N->getLocation()); }
  void visitCall(CallExpr *C);
  void visitSubscript(SubscriptExpr *SB);
  void ResolveName(const std::string &Name, Location L);

  /// Trivial setters for important states during the construction
  /// of a table.
  void setFuncDef(FuncDef *FD) { TheFuncDef = FD; }
  void setTable(SymbolTable *ST) { TheTable = ST; }
  void setLocal(TableType *L) { TheLocal = L; }
  void setGlobal(TableType *G) { TheGlobal = G; }

  /// Clear the state of this SymbolTableBuilder
  void clear();

public:
  /// Default construct and destruct.
  SymbolTableBuilder() { clear(); }

  /// Build a SymbolTable from a program.
  /// Return true if errors happened.
  /// Note: the table will be cleared first.
  bool Build(ProgramAST *P, SymbolTable &S);

private:
  friend VisitorBase;
  friend ChildrenVisitor;

  ErrorManager EM;
  TableType *TheGlobal;
  TableType *TheLocal;
  FuncDef *TheFuncDef;
  SymbolTable *TheTable;
};
} // namespace simplecc
#endif // SIMPLECC_ANALYSIS_SYMBOLTABLEBUILDER_H