#ifndef SIMPLECC_ANALYSIS_ANALYSISVISITOR_H
#define SIMPLECC_ANALYSIS_ANALYSISVISITOR_H
#include "simplecc/Analysis/Visitor.h"
#include "simplecc/Analysis/SymbolTable.h"
#include "simplecc/Support/ErrorManager.h"
#include <utility>
#include <string>

namespace simplecc {
/// This class is a CRTP base for classes that perform analysis on the AST.
/// It sets up the corresponding LocalSymbolTable for each FuncDef to be visited.
/// It also acts as an ErrorManger so that subclass don't need to own one.
/// It provides a Check() method that means to streamline the interface of subclasses.
/// Due to CRTP, subclasses should:
/// 1. private inherit this class.
/// 2. make friends with all its Visitor-like base classes. (Currently 3).
/// 3. mark Check() as public.
/// Note: subclasses are **allowed** to mutate both the AST and the SymbolTable.
template<typename Derived>
class AnalysisVisitor : public ChildrenVisitor<Derived>, public ErrorManager {
public:
  AnalysisVisitor(const char *ErrorType = nullptr)
      : ErrorManager(ErrorType), TheTable(), TheLocalTable() {}

  /// Public interface. Subclass should mark it as public.
  bool Check(Program *P, SymbolTable &S) {
    setTable(S);
    static_cast<Derived *>(this)->visitProgram(P);
    return !IsOk();
  }

protected:
  /// Access to the SymbolTable.
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

  /// Access to the LocalSymbolTable per FuncDef basic.
  void setLocalTable(FuncDef *FD) {
    TheLocalTable = getSymbolTable().getLocalTable(FD);
  }
  LocalSymbolTable getLocalTable() const {
    return TheLocalTable;
  }
  SymbolEntry getSymbolEntry(const std::string &Name) const {
    return TheLocalTable[Name];
  }

  /// Initialize LocalSymbolTable before performing real visit.
  /// Subclass can override it and add more initialization.
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
