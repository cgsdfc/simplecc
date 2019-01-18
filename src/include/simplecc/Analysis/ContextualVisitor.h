#ifndef SIMPLECC_ANALYSIS_CONTEXTUALVISITOR_H
#define SIMPLECC_ANALYSIS_CONTEXTUALVISITOR_H
#include "simplecc/Analysis/SymbolTable.h"
#include "simplecc/Analysis/Visitor.h"

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