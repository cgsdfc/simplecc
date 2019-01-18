#ifndef SIMPLECC_ANALYSIS_ANALYSISVISITOR_H
#define SIMPLECC_ANALYSIS_ANALYSISVISITOR_H
#include "simplecc/Analysis/ContextualVisitor.h"
#include "simplecc/Support/ErrorManager.h"
#include <utility>

namespace simplecc {
/// @brief AnalysisVisitor is a CRTP base for classes that perform an analysis on the AST.
/// It also acts as an ErrorManger so that subclass don't need to own one.
/// It provides a Check() method that means to streamline the interface of
/// subclasses. Due to CRTP, subclasses should:
/// 1. private inherit this class.
/// 2. make friends with all its Visitor-like base classes. (Currently 3).
/// 3. mark Check() as public.
/// Note: subclasses are **allowed** to mutate both the AST and the SymbolTable.
template <typename Derived>
class AnalysisVisitor : public ContextualVisitor<Derived>, public ErrorManager {
public:
  explicit AnalysisVisitor(const char *ErrorType = nullptr)
      : ErrorManager(ErrorType) {}

  /// Perform a check on the program.
  bool Check(ProgramAST *P, SymbolTable &S) {
    ContextualVisitor<Derived>::visitProgram(P, S);
    return !IsOk();
  }
};

} // namespace simplecc
#endif // SIMPLECC_ANALYSIS_ANALYSISVISITOR_H