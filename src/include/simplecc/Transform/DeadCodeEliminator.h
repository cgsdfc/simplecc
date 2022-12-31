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

#ifndef SIMPLECC_TRANSFORM_TRIVIALDCETRANSFORMER_H
#define SIMPLECC_TRANSFORM_TRIVIALDCETRANSFORMER_H
#include "simplecc/AST/ChildrenVisitor.h"

namespace simplecc {
/// This class implements a trivial dead code elimination (DCE) transformation
/// on the AST. It basically performs these operations:
/// 1. Delete stmts in a stmt list that appear after a return-stmt.
/// 2. For an if-stmt, if its condition is a constant, then replace it with
/// either branch that will actually execute.
/// 3. For a while-stmt, if its condition is false, delete it.
/// 4. For a for-stmt, if its condition is false, do the following:
/// for (initial; condition; step) { body } becomes:
/// initial; body; step;
/// This transformation requires the TrivialConstantFolder to run first.
class DeadCodeEliminator : ChildrenVisitor<DeadCodeEliminator> {
  friend VisitorBase;
  friend ChildrenVisitor;
  using StmtListType = std::vector<StmtAST *>;
  /// Run the DCE algorithm once on the stmt list, return if
  /// the list was mutated.
  void TransformStmtList(StmtListType &StmtList);
  void visitStmtList(StmtListType &StmtList);
  void visitFor(ForStmt *F);
  void visitWhile(WhileStmt *W);
  void visitIf(IfStmt *I);
  void visitFuncDef(FuncDef *FD);

public:
  DeadCodeEliminator() = default;
  void Transform(ProgramAST *P) { return ChildrenVisitor::visitProgram(P); }
};
} // namespace simplecc
#endif // SIMPLECC_TRANSFORM_TRIVIALDCETRANSFORMER_H