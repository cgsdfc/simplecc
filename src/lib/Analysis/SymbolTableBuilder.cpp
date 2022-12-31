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

#include "simplecc/Analysis/SymbolTableBuilder.h"

using namespace simplecc;

void SymbolTableBuilder::DefineLocalDecl(DeclAST *D) {
  assert(TheLocal && "TheLocal must be set!");
  if (TheLocal->count(D->getName())) {
    EM.Error(D->getLocation(), "redefinition of identifier", D->getName(), "in",
             TheFuncDef->getName());
    return;
  }
  if (TheGlobal->count(D->getName()) &&
      (*TheGlobal)[D->getName()].IsFunction()) {
    EM.Error(D->getLocation(), "local identifier", D->getName(), "in",
             TheFuncDef->getName(), "shallows a global function");
    return;
  }
  /// Now we successfully define the name
  TheLocal->emplace(D->getName(), SymbolEntry(Scope::Local, D));
}

void SymbolTableBuilder::DefineGlobalDecl(DeclAST *D) {
  assert(TheGlobal && "TheGlobal must be set!");
  if (TheGlobal->count(D->getName())) {
    EM.Error(D->getLocation(), "redefinition of identifier", D->getName(),
             "in <module>");
    return;
  }
  TheGlobal->emplace(D->getName(), SymbolEntry(Scope::Global, D));
}

void SymbolTableBuilder::ResolveName(const std::string &Name, Location L) {
  assert(TheLocal && TheGlobal && TheFuncDef);
  if (TheLocal->count(Name))
    return;
  if (TheGlobal->count(Name)) {
    /// Fall back to globally.
    TheLocal->emplace(Name, (*TheGlobal)[Name]);
    return;
  }
  /// Undefined
  EM.Error(L, "undefined identifier", Name, "in", TheFuncDef->getName());
}

void SymbolTableBuilder::visitCall(CallExpr *C) {
  ResolveName(C->getCallee(), C->getLocation());
  /// Recurse into children.
  ChildrenVisitor::visitCall(C);
}

void SymbolTableBuilder::visitSubscript(SubscriptExpr *SB) {
  ResolveName(SB->getArrayName(), SB->getLocation());
  /// Recurse into children.
  ChildrenVisitor::visitSubscript(SB);
}

void SymbolTableBuilder::visitDecl(DeclAST *D) {
  switch (D->getKind()) {
  case DeclAST::FuncDefKind:
    setFuncDef(static_cast<FuncDef *>(D));
    setLocal(&TheTable->getLocal(TheFuncDef));
    /// Define this function globally.
    DefineGlobalDecl(D);
    return visitFuncDef(TheFuncDef);

  case DeclAST::ConstDeclKind:
  case DeclAST::VarDeclKind:
  case DeclAST::ArgDeclKind:
    /* Fall through */
    return TheLocal ? DefineLocalDecl(D) : DefineGlobalDecl(D);
  default:
    assert(false && "Unhandled DeclAST subclass!");
  }
}

bool SymbolTableBuilder::Build(ProgramAST *P, SymbolTable &S) {
  clear();
  S.clear();
  setTable(&S);
  setGlobal(&S.getGlobal());
  EM.setErrorType("NameError");
  visitProgram(P);
  return !EM.IsOk();
}

void SymbolTableBuilder::clear() {
  setTable(nullptr);
  setLocal(nullptr);
  setGlobal(nullptr);
  setFuncDef(nullptr);
  EM.clear();
}