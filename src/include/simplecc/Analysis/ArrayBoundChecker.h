#ifndef SIMPLECC_ANALYSIS_ARRAYBOUNDCHECKER_H
#define SIMPLECC_ANALYSIS_ARRAYBOUNDCHECKER_H
#include "simplecc/Analysis/SymbolTable.h"
#include "simplecc/Analysis/Visitor.h"
#include "simplecc/Support/ErrorManager.h"
#include "simplecc/Analysis/AnalysisVisitor.h"
#include <utility> // for pair

namespace simplecc {
class ArrayBoundChecker : AnalysisVisitor<ArrayBoundChecker> {
  friend AnalysisVisitor;
  friend ChildrenVisitor;
  friend VisitorBase;
  std::pair<bool, int> getIndex(Expr *E) const;
  void visitSubscript(SubscriptExpr *SB);
public:
  ArrayBoundChecker() = default;
  using AnalysisVisitor::Check;
};
} // namespace simplecc

#endif // SIMPLECC_ANALYSIS_ARRAYBOUNDCHECKER_H
