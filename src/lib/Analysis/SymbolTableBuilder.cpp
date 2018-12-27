#include "simplecc/Analysis/SymbolTableBuilder.h"

using namespace simplecc;

void SymbolTableBuilder::DefineLocalDecl(Decl *D) {
  assert(TheLocal && "TheLocal must be set!");
  if (TheLocal->count(D->getName())) {
    EM.Error(D->getLoc(), "redefinition of identifier", D->getName(),
             "in function", TheFuncDef->getName());
    return;
  }
  if (TheGlobal->count(D->getName()) &&
      (*TheGlobal)[D->getName()].IsFunction()) {
    EM.Error(D->getLoc(), "local identifier", D->getName(), "in function",
             TheFuncDef->getName(), "shallows a global function");
    return;
  }
  /// Now we successfully define the name
  TheLocal->emplace(D->getName(), SymbolEntry(Scope::Local, D));
}

void SymbolTableBuilder::DefineGlobalDecl(Decl *D) {
  assert(TheGlobal && "TheGlobal must be set!");
  if (TheGlobal->count(D->getName())) {
    EM.Error(D->getLoc(), "redefinition of identifier", D->getName(),
             "in <module>");
    return;
  }
  TheGlobal->emplace(D->getName(), SymbolEntry(Scope::Global, D));
}

void SymbolTableBuilder::ResolveName(const String &Name, const Location &L) {
  assert(TheLocal && TheGlobal && TheFuncDef);
  if (TheLocal->count(Name))
    return;
  if (TheGlobal->count(Name)) {
    /// Fall back to globally.
    TheLocal->emplace(Name, (*TheGlobal)[Name]);
    return;
  }
  /// Undefined
  EM.Error(L, "undefined identifier", Name, "in function",
           TheFuncDef->getName());
}

void SymbolTableBuilder::visitCall(CallExpr *C) {
  ResolveName(C->getFunc(), C->getLoc());
  /// Recurse into children.
  ChildrenVisitor::visitCall(C);
}

void SymbolTableBuilder::visitSubscript(SubscriptExpr *SB) {
  ResolveName(SB->getName(), SB->getLoc());
  /// Recurse into children.
  ChildrenVisitor::visitSubscript(SB);
}

void SymbolTableBuilder::visitDecl(Decl *D) {
  switch (D->GetKind()) {
  case Decl::FuncDef:
    setFuncDef(static_cast<FuncDef *>(D));
    setLocal(&TheTable->getLocal(TheFuncDef));
    /// Define this function globally.
    DefineGlobalDecl(D);
    return visitFuncDef(TheFuncDef);

  case Decl::ConstDecl:
  case Decl::VarDecl:
  case Decl::ArgDecl:
    /* Fall through */
    return TheLocal ? DefineLocalDecl(D) : DefineGlobalDecl(D);
  default:
    assert(false && "Unhandled Decl subclass!");
  }
}

bool SymbolTableBuilder::Build(Program *P, SymbolTable &S) {
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