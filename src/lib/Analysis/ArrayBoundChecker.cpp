#include "simplecc/Analysis/ArrayBoundChecker.h"

using namespace simplecc;

void ArrayBoundChecker::visitSubscript(SubscriptExpr *SB) {
  SymbolEntry Entry(TheLocalTable[SB->getName()]);
  if (!Entry.IsArray()) {
    return;
  }
  ArrayType AT(Entry.AsArray());
  std::pair<bool, int> Idx = getIndex(SB->getIndex());
  if (!Idx.first)
    return;
  int Val = Idx.second;
  if (Val < 0 || Val >= AT.getSize()) {
    EM.Error(SB->getLoc(), "array index out of bound:", Val);
  }
}

std::pair<bool, int> ArrayBoundChecker::getIndex(Expr *E) const {
  // ReturnStmt this if E is not a static index.
  std::pair<bool, int> False(false, 0);
  // Case-1: NumExpr.
  if (IsInstance<NumExpr>(E)) {
    return std::make_pair(true, static_cast<NumExpr *>(E)->getN());
  }

  // Case-2: UnaryOpExpr[NumExpr].
  if (IsInstance<UnaryOpExpr>(E) &&
      IsInstance<NumExpr>(static_cast<UnaryOpExpr *>(E)->getOperand())) {
    auto Extract = [](UnaryOpExpr *U) {
      auto Val = static_cast<NumExpr *>(U->getOperand())->getN();
      return U->getOp() == UnaryopKind::USub ? -Val : Val;
    };
    return std::make_pair(true, Extract(static_cast<UnaryOpExpr *>(E)));
  }

  // Case-3: const int NameConstant = 1;
  if (!IsInstance<NameExpr>(E))
    return False;

  auto N = static_cast<NameExpr *>(E);
  SymbolEntry Entry(TheLocalTable[N->getId()]);
  if (!Entry.IsConstant())
    return False;

  ConstType CT(Entry.AsConstant());
  if (CT.getType() != BasicTypeKind::Int)
    return False;

  return std::make_pair(true, CT.getValue());
}

void ArrayBoundChecker::visitFuncDef(FuncDef *FD) {
  assert(TheTable && "TheTable must be set");
  TheLocalTable = TheTable->getLocalTable(FD);
  ChildrenVisitor::visitFuncDef(FD);
}

bool ArrayBoundChecker::Check(Program *P, const SymbolTable &S) {
  TheTable = &S;
  visitProgram(P);
  return !EM.IsOk();
}