#include <simplecc/Transform/DeadCodeEliminator.h>

using namespace simplecc;

void DeadCodeEliminator::visitFor(ForStmt *F) {
  visitStmtList(F->getBody());
}

void DeadCodeEliminator::visitWhile(WhileStmt *W) {
  visitStmtList(W->getBody());
}

void DeadCodeEliminator::visitIf(IfStmt *I) {
  visitStmtList(I->getBody());
  visitStmtList(I->getOrelse());
}

void DeadCodeEliminator::visitFuncDef(FuncDef *FD) {
  visitStmtList(FD->getStmts());
}

void DeadCodeEliminator::visitStmtList(StmtListType &StmtList) {
  // Transform each child first.
  std::for_each(StmtList.begin(), StmtList.end(), [this](StmtAST *Stmt) {
    visitStmt(Stmt);
  });
  TransformStmtList(StmtList);
}

void DeadCodeEliminator::TransformStmtList(StmtListType &StmtList) {
  for (auto Iter = StmtList.begin(); Iter != StmtList.end();) {
    // Case-1: find the first return-stmt and delete everything after it.
    if (IsInstance<ReturnStmt>(*Iter)) {
      std::advance(Iter, 1);
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
    // Case-3: if-stmt. replace it with its either branch if the condition is constant.
    /// if (false) then; else; becomes
    /// else;
    if (IsInstance<IfStmt>(*Iter) &&
        static_cast<IfStmt *>(*Iter)->getTest()->isConstant()) {
      auto If = static_cast<IfStmt *>(*Iter);
      std::vector<StmtAST *> Branch = If->getTest()->isZeroVal() ?
                                      std::move(*If).getOrelse() : std::move(*If).getBody();
      DeleteAST::apply(If);
      Iter = StmtList.erase(Iter);
      Iter = StmtList.insert(Iter, Branch.begin(), Branch.end());
      Iter += Branch.size();
      continue;
    }
    // Case-4: for-stmt.
    if (IsInstance<ForStmt>(*Iter) &&
        static_cast<ForStmt *>(*Iter)->getCondition()->isZeroVal()) {
      auto For = static_cast<ForStmt *>(*Iter);
      auto Initial = std::move(*For).getInitial();
      auto Step = std::move(*For).getStep();
      std::vector<StmtAST *> Body = std::move(*For).getBody();
      DeleteAST::apply(For);
      *Iter = static_cast<StmtAST *>(Initial.release());
      StmtList.insert(next(Iter), Body.begin(), Body.end());
      Iter += Body.size();
      *Iter++ = static_cast<StmtAST *>(Step.release());
      continue;
    }
    ++Iter;
  }
}