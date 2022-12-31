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