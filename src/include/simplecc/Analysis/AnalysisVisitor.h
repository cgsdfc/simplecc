#ifndef SIMPLECC_ANALYSIS_ANALYSISVISITOR_H
#define SIMPLECC_ANALYSIS_ANALYSISVISITOR_H
#include "simplecc/Analysis/Visitor.h"
#include "simplecc/Analysis/SymbolTable.h"
#include "simplecc/Support/ErrorManager.h"
#include <utility>
#include <string>

namespace simplecc {
/// This class is a CRTP base for classes that perform analysis on the AST.
/// subclasses are **allowed** to mutate both the AST and the SymbolTable.
template<typename Derived>
class AnalysisVisitor : public ChildrenVisitor<Derived> {
public:
  AnalysisVisitor(const char *ErrorType = nullptr)
      : TheTable(), TheLocalTable(), EM(ErrorType) {}

  bool Check(Program *P, SymbolTable &S) {
    setTable(S);
    static_cast<Derived *>(this)->visitProgram(P);
    return !EM.IsOk();
  }

protected:
  void setTable(SymbolTable &S) {
    TheTable = &S;
  }
  const SymbolTable &getSymbolTable() const {
    assert(TheTable);
    return *TheTable;
  }
  SymbolTable &getSymbolTable() {
    assert(TheTable);
    return *TheTable;
  }

  void setLocalTable(FuncDef *FD) {
    TheLocalTable = getSymbolTable().getLocalTable(FD);
  }
  LocalSymbolTable getLocalTable() const {
    return TheLocalTable;
  }
  SymbolEntry getSymbolEntry(const std::string &Name) const {
    return TheLocalTable[Name];
  }

  /// Forward to EM.Error()
  template<typename ... Ts>
  void Error(Location L, Ts &&... Args) {
    EM.Error(L, std::forward<Ts>(Args)...);
  }

  void visitFuncDef(FuncDef *FD) {
    setLocalTable(FD);
    ChildrenVisitor<Derived>::visitFuncDef(FD);
  }

private:
  SymbolTable *TheTable;
  LocalSymbolTable TheLocalTable;
  ErrorManager EM;
};

}
#endif //SIMPLECC_ANALYSIS_ANALYSISVISITOR_H
