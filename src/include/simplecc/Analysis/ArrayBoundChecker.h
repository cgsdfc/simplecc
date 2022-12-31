// MIT License
// 
// Copyright (c) 2018 Cong Feng.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef SIMPLECC_ANALYSIS_ARRAYBOUNDCHECKER_H
#define SIMPLECC_ANALYSIS_ARRAYBOUNDCHECKER_H
#include "simplecc/Analysis/AnalysisVisitor.h"
#include "simplecc/Analysis/SymbolTable.h"
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