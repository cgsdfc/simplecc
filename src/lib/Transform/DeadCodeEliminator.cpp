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

#include <simplecc/Transform/DeadCodeEliminator.h>

using namespace simplecc;

void DeadCodeEliminator::visitFor(ForStmt *F) { visitStmtList(F->getBody()); }

void DeadCodeEliminator::visitWhile(WhileStmt *W) {
  visitStmtList(W->getBody());
}

void DeadCodeEliminator::visitIf(IfStmt *I) {
  visitStmtList(I->getThen());
  visitStmtList(I->getElse());
}

void DeadCodeEliminator::visitFuncDef(FuncDef *FD) {
  visitStmtList(FD->getStmts());
}

void DeadCodeEliminator::visitStmtList(StmtListType &StmtList) {
  // Transform each child first.
  std::for_each(StmtList.begin(), StmtList.end(),
                [this](StmtAST *Stmt) { visitStmt(Stmt); });
  TransformStmtList(StmtList);
}

void DeadCodeEliminator::TransformStmtList(StmtListType &StmtList) {
  for (auto Iter = StmtList.begin(); Iter != StmtList.end();) {
    // Case-1: find the first return-stmt and delete everything after it.
    if (IsInstance<ReturnStmt>(*Iter)) {
      // Start from the next stmt of return-stmt.
      std::advance(Iter, 1);
      // Delete and erase a range.
      DeleteAST::apply(Iter, StmtList.end());
      StmtList.erase(Iter, StmtList.end());
      break;
    }
    // Case-2: find any while-stmt with false condition, delete them.
    if (IsInstance<WhileStmt>(*Iter) &&
        static_cast<WhileStmt *>(*Iter)->getCondition()->isZeroVal()) {
      DeleteAST::apply(*Iter);
      Iter = StmtList.erase(Iter);
      continue;
    }
    // Case-3: if-stmt. replace it with its either branch if the condition is
    // constant.
    /// if (false) then; else; becomes
    /// else;
    if (IsInstance<IfStmt>(*Iter) &&
        static_cast<IfStmt *>(*Iter)->getCondition()->isConstant()) {
      auto If = static_cast<IfStmt *>(*Iter);
      std::vector<StmtAST *> Branch = If->getCondition()->isZeroVal()
                                          ? std::move(*If).getElse()
                                          : std::move(*If).getThen();
      // Delete and erase the if-stmt.
      DeleteAST::apply(If);
      Iter = StmtList.erase(Iter);

      // Replace with either Then or Else branch.
      Iter = StmtList.insert(Iter, Branch.begin(), Branch.end());
      std::advance(Iter, Branch.size());
      continue;
    }
    // Case-4: for-stmt. for (initial; condition; step) body =>
    // initial; body; step;
    if (IsInstance<ForStmt>(*Iter) &&
        static_cast<ForStmt *>(*Iter)->getCondition()->isZeroVal()) {
      auto For = static_cast<ForStmt *>(*Iter);
      auto Initial = std::move(*For).getInitial();
      auto Step = std::move(*For).getStep();
      std::vector<StmtAST *> Body = std::move(*For).getBody();
      DeleteAST::apply(For);
      // Note: Be careful with *Iter++ = thing!
      // The container won't resize as you write to it via iterator, So when
      // Iter went out of range (end()), the container was compromised.

      // Note: This is replacing the old ForStmt at Iter with its Initial and
      // advance Iter by one. Since Iter originally pointed to valid element,
      // Iter+1 is always valid (even it is end()). Thus it can be passed to
      // insert(). Here, *Iter is valid storage and Iter+1 is valid iterator so
      // it is a valid construct.
      *Iter++ = Initial.release();
      Iter = StmtList.insert(Iter, Body.begin(), Body.end());
      // Advance to the end of the Body stmt list.
      std::advance(Iter, Body.size());
      // Insert the Step stmt right after the Body.
      Iter = StmtList.insert(Iter, Step.release());
      ++Iter;
      continue;
    }
    ++Iter;
  }
}