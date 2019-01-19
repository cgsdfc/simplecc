#ifndef SIMPLECC_ANALYSIS_ARRAYBOUNDCHECKER_H
#define SIMPLECC_ANALYSIS_ARRAYBOUNDCHECKER_H
#include "simplecc/Analysis/AnalysisVisitor.h"
#include "simplecc/Analysis/SymbolTable.h"
#include "simplecc/AST/Visitor.h"
#include "simplecc/Support/ErrorManager.h"
#include <utility> // for pair

namespace simplecc {
/// @brief ArrayBoundChecker performs static array bound check on a program.
/// For any SubscriptExpr in a program, if its index is a simple constant expression
/// then it checks whether the index goes out of bound.
/// Simple constant expression means it is a:
/// 1. NumExpr.
/// 2. UnaryOpExpr wrapping a NumExpr, e.g., -1.
/// 3. NameExpr that is a ConstType.
/// Index other than cases above is excluded from this check.
class ArrayBoundChecker : AnalysisVisitor<ArrayBoundChecker> {
  /// CRTP boilerplate.
  friend AnalysisVisitor;
  friend ChildrenVisitor;
  friend VisitorBase;
  std::pair<bool, int> getIndex(ExprAST *E) const;
  void visitSubscript(SubscriptExpr *SB);

public:
  ArrayBoundChecker() = default;
  /// Perform the check.
  using AnalysisVisitor::Check;
};
} // namespace simplecc

#endif // SIMPLECC_ANALYSIS_ARRAYBOUNDCHECKER_H