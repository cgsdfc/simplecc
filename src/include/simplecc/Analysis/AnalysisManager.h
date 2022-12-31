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

/// @file External interface to the Analysis module.
#ifndef SIMPLECC_ANALYSIS_ANALYSIS_H
#define SIMPLECC_ANALYSIS_ANALYSIS_H
#include "simplecc/Analysis/SymbolTable.h"
#include "simplecc/Analysis/Types.h"

// TODO: this class sounds very silly.

namespace simplecc {
/// @brief AnalysisManager hides the details of all analyses and provides a simple
/// interface to run all analyses on a program.
class AnalysisManager {
  SymbolTable TheTable;

public:
  AnalysisManager() = default;
  ~AnalysisManager();

  /// Run **all** the analyses on the Program.
  /// Return true if errors happened.
  bool runAllAnalyses(ProgramAST *P);

  /// Return the symbol table backing the analyses.
  const SymbolTable &getSymbolTable() const { return TheTable; }

  SymbolTable &getSymbolTable() { return TheTable; }

  void clear() { TheTable.clear(); }
};
} // namespace simplecc

#endif // SIMPLECC_ANALYSIS_ANALYSIS_H