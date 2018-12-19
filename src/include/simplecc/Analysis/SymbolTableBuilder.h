#ifndef SIMPLECOMPILER_SYMBOLTABLEBUILDER_H
#define SIMPLECOMPILER_SYMBOLTABLEBUILDER_H
#include "Visitor.h"
#include "simplecc/Support/ErrorManager.h"
#include "SymbolTable.h"

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
  friend class VisitorBase<SymbolTableBuilder>;
  friend class ChildrenVisitor<SymbolTableBuilder>;

  ErrorManager EM;
  TableType *TheGlobal;
  TableType *TheLocal;
  FuncDef *TheFuncDef;
  SymbolTable *TheTable;

  void DefineLocalDecl(Decl *D);

  void DefineGlobalDecl(Decl *D);

  void ResolveName(const String &Name, const Location &L);

  /// Overloads to visit AstNodes that have names.
  void visitName(Name *N) { ResolveName(N->getId(), N->getLoc()); }

  void visitCall(Call *C);

  void visitSubscript(Subscript *SB);

  void visitDecl(Decl *D);
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
  SymbolTableBuilder() = default;
  ~SymbolTableBuilder() = default;

  /// Trivially copyable and moveable.
  SymbolTableBuilder(const SymbolTableBuilder &) = default;
  SymbolTableBuilder(SymbolTableBuilder &&) = default;

  /// Build a SymbolTable from a program.
  /// Return OK or not.
  /// Note: the table will be cleared first.
  bool Build(Program *P, SymbolTable &S);
};
}

#endif //SIMPLECOMPILER_SYMBOLTABLEBUILDER_H
