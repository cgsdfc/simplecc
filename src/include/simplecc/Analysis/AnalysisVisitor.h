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
class AnalysisVisitor : public ChildrenVisitor<Derived>, public ErrorManager {
public:
  AnalysisVisitor(const char *ErrorType = nullptr)
      : ErrorManager(ErrorType), TheTable(), TheLocalTable() {}

  bool Check(Program *P, SymbolTable &S) {
    setTable(S);
    static_cast<Derived *>(this)->visitProgram(P);
    return !IsOk();
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

  void visitFuncDef(FuncDef *FD) {
    setLocalTable(FD);
    ChildrenVisitor<Derived>::visitFuncDef(FD);
  }

private:
  SymbolTable *TheTable;
  LocalSymbolTable TheLocalTable;
};

}
#endif //SIMPLECC_ANALYSIS_ANALYSISVISITOR_H
