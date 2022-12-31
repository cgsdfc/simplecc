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

#include "simplecc/Analysis/AnalysisManager.h"
#include "simplecc/AST/ASTVerifier.h"
#include "simplecc/Analysis/ArrayBoundChecker.h"
#include "simplecc/Analysis/ImplicitCallTransformer.h"
#include "simplecc/Analysis/SymbolTableBuilder.h"
#include "simplecc/Analysis/SyntaxChecker.h"
#include "simplecc/Analysis/TypeChecker.h"

using namespace simplecc;

AnalysisManager::~AnalysisManager() = default;

bool AnalysisManager::runAllAnalyses(ProgramAST *P) {
  if (SyntaxChecker().Check(P)) {
    return true;
  }

  if (SymbolTableBuilder().Build(P, TheTable)) {
    return true;
  }

  ImplicitCallTransformer().Transform(P, TheTable);

  if (TypeChecker().Check(P, TheTable)) {
    return true;
  }

  if (ArrayBoundChecker().Check(P, TheTable)) {
    return true;
  }

  if (ASTVerifier().Check(P)) {
    PrintErrs("ProgramAST should be well-formed after all analyses run!");
    return true;
  }

  return false;
}