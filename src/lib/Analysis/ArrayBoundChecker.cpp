#include "simplecc/Analysis/ArrayBoundChecker.h"

using namespace simplecc;

void ArrayBoundChecker::visitSubscript(Subscript *SB) {
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
  // Return this if E is not a static index.
  std::pair<bool, int> False(false, 0);
  // Case-1: Num.
  if (IsInstance<Num>(E)) {
    return std::make_pair(true, static_cast<Num *>(E)->getN());
  }

  // Case-2: UnaryOp[Num].
  if (IsInstance<UnaryOp>(E) &&
      IsInstance<Num>(static_cast<UnaryOp *>(E)->getOperand())) {
    auto Extract = [](UnaryOp *U) {
      auto Val = static_cast<Num *>(U->getOperand())->getN();
      return U->getOp() == UnaryopKind::USub ? -Val : Val;
    };
    return std::make_pair(true, Extract(static_cast<UnaryOp *>(E)));
  }

  // Case-3: const int NameConstant = 1;
  if (!IsInstance<Name>(E))
    return False;

  auto N = static_cast<Name *>(E);
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