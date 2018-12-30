#ifndef SIMPLECC_ANALYSIS_ANALYSISVISITOR_H
#define SIMPLECC_ANALYSIS_ANALYSISVISITOR_H
#include "simplecc/Analysis/ContextualVisitor.h"
#include "simplecc/Support/ErrorManager.h"
#include <utility>

namespace simplecc {
/// This class is a CRTP base for classes that perform analysis on the AST.
/// It also acts as an ErrorManger so that subclass don't need to own one.
/// It provides a Check() method that means to streamline the interface of subclasses.
/// Due to CRTP, subclasses should:
/// 1. private inherit this class.
/// 2. make friends with all its Visitor-like base classes. (Currently 3).
/// 3. mark Check() as public.
/// Note: subclasses are **allowed** to mutate both the AST and the SymbolTable.
template<typename Derived>
class AnalysisVisitor : public ContextualVisitor<Derived>, public ErrorManager {
public:
  explicit AnalysisVisitor(const char *ErrorType = nullptr)
      : ErrorManager(ErrorType) {}

  /// Public interface. Subclass should mark it as public.
  bool Check(Program *P, SymbolTable &S) {
    ContextualVisitor<Derived>::visitProgram(P, S);
    return !IsOk();
  }
};

}
#endif //SIMPLECC_ANALYSIS_ANALYSISVISITOR_H
