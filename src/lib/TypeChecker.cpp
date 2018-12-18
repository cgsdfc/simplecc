#include "simplecc/TypeChecker.h"
#include <algorithm>

using namespace simplecc;

namespace simplecc {
bool CheckType(Program *P, SymbolTable &S) { return TypeChecker().Check(P, S); }
} // namespace simplecc


bool TypeChecker::Check(Program *P, SymbolTable &S) {
  EM.setErrorType("TypeError");
  setTable(&S);
  visitProgram(P);
  return EM.IsOk();
}

void TypeChecker::visitRead(Read *RD) {
  for (auto E : RD->getNames()) {
    auto N = subclass_cast<Name>(E);
    assert(N);
    const auto &Entry = TheLocalTable[N->getId()];
    if (!Entry.IsVariable()) {
      EM.Error(N->getLoc(), "cannot use scanf() on", Entry.getTypeName(),
                   Entry.getName());
      continue;
    }
    /// set the Expr type for this.
    TheTable->setExprType(E, Entry.AsVariable().getType());
  }
}

void TypeChecker::visitWrite(Write *WR) {
  if (WR->getValue()) {
    CheckExprOperand(WR->getValue());
  }
}

void TypeChecker::visitReturn(Return *R) {
  auto ActuallyReturn =
      R->getValue() ? visitExpr(R->getValue()) : BasicTypeKind::Void;
  auto ShouldReturn = TheFuncDef->getReturnType();

  // order a strict match
  if (ShouldReturn != ActuallyReturn) {
    EM.Error(R->getLoc(), "return type mismatched:", "function",
                 Quote(TheFuncDef->getName()), "must return",
                 CStringFromBasicTypeKind(ShouldReturn), "not",
                 CStringFromBasicTypeKind(ActuallyReturn));
  }
}

void TypeChecker::visitAssign(Assign *A) {
  int Errs = EM.getErrorCount();
  auto RHS = CheckExprOperand(A->getValue());
  auto LHS = visitExpr(A->getTarget());

  if (EM.IsOk(Errs) && LHS != RHS) {
    EM.Error(A->getLoc(), "type mismatched in assignment:",
                 CStringFromBasicTypeKind(LHS), "=",
                 CStringFromBasicTypeKind(RHS));
  }
}

// check the operand of BoolOp, restrict to int
void TypeChecker::CheckBoolOpOperand(Expr *E) {
  auto Msg = "operands of condition must be of type int";
  auto Errs = EM.getErrorCount();
  auto T = visitExpr(E);
  if (EM.IsOk(Errs) && T != BasicTypeKind::Int) {
    EM.Error(E->getLoc(), Msg);
  }
}

BasicTypeKind TypeChecker::visitBoolOp(BoolOp *B) {
  if (B->getHasCmpop()) {
    auto Bin = static_cast<BinOp *>(B->getValue());
    CheckBoolOpOperand(Bin->getLeft());
    CheckBoolOpOperand(Bin->getRight());
  } else {
    CheckBoolOpOperand(B->getValue());
  }
  return BasicTypeKind::Int;
}

// check the operand of Expr, restrict to NOT void
BasicTypeKind TypeChecker::CheckExprOperand(Expr *E) {
  auto Msg = "void value cannot be used in an expression";
  auto Errs = EM.getErrorCount();
  auto T = visitExpr(E);
  if (EM.IsOk(Errs) && T == BasicTypeKind::Void) {
    EM.Error(E->getLoc(), Msg);
  }
  return T;
}

BasicTypeKind TypeChecker::visitBinOp(BinOp *B) {
  CheckExprOperand(B->getLeft());
  CheckExprOperand(B->getRight());
  return BasicTypeKind::Int;
}

BasicTypeKind TypeChecker::visitUnaryOp(UnaryOp *U) {
  CheckExprOperand(U->getOperand());
  return BasicTypeKind::Int;
}

BasicTypeKind TypeChecker::visitParenExpr(ParenExpr *PE) {
  CheckExprOperand(PE->getValue());
  return BasicTypeKind::Int;
}

BasicTypeKind TypeChecker::visitCall(Call *C) {
  const auto &Entry = TheLocalTable[C->getFunc()];
  if (!Entry.IsFunction()) {
    EM.Error(C->getLoc(), Entry.getTypeName(), Entry.getName(),
                 "cannot be called as a function");
    return BasicTypeKind::Void;
  }

  auto Ty = Entry.AsFunction();
  auto NumFormal = Ty.getArgCount();
  auto NumActual = C->getArgs().size();
  if (NumFormal != NumActual) {
    EM.Error(C->getLoc(), "function", Quote(C->getFunc()), "expects",
                 NumFormal, "arguments, got", NumActual);
  }

  // check args
  auto Size = std::min(NumFormal, NumActual);
  for (int I = 0; I < Size; I++) {
    auto ActualTy = visitExpr(C->getArgs()[I]);
    auto FormalTy = Ty.getArgTypeAt(I);
    if (ActualTy != FormalTy) {
      EM.Error(C->getArgs()[I]->getLoc(), "argument", I + 1,
                   "of function", Quote(C->getFunc()), "must be",
                   CStringFromBasicTypeKind(FormalTy), ", not",
                   CStringFromBasicTypeKind(ActualTy));
    }
  }
  return Ty.getReturnType();
}

BasicTypeKind TypeChecker::visitSubscript(Subscript *SB) {
  const auto &Entry = TheLocalTable[SB->getName()];
  if (!Entry.IsArray()) {
    EM.Error(SB->getLoc(), Entry.getTypeName(), Entry.getName(),
                 "cannot be subscripted as an array");
    return BasicTypeKind::Void;
  }

  int Errs = EM.getErrorCount();
  auto Idx = visitExpr(SB->getIndex());
  if (EM.IsOk(Errs) && Idx != BasicTypeKind::Int) {
    EM.Error(SB->getLoc(), "Array index must be int");
  }
  return Entry.AsArray().getElementType();
}

BasicTypeKind TypeChecker::visitName(Name *N) {
  const auto &Entry = TheLocalTable[N->getId()];
  if (N->getCtx() == ExprContextKind::Load && Entry.IsArray()) {
    EM.Error(N->getLoc(), Entry.getTypeName(), Entry.getName(),
                 "cannot be used in an expression");
    return BasicTypeKind::Void;
  }
  if (N->getCtx() == ExprContextKind::Store && !Entry.IsVariable()) {
    EM.Error(N->getLoc(), Entry.getTypeName(), Entry.getName(),
                 "cannot be assigned to");
    return BasicTypeKind::Void;
  }
  if (Entry.IsConstant())
    return Entry.AsConstant().getType();
  return Entry.AsVariable().getType();
}

// not actually used, for instantiation only
BasicTypeKind TypeChecker::visitStr(Str *) {
  assert(false && "TypeCheker::visitStr() shall not be called");
  return BasicTypeKind::Void;
}

// Return the type of evaluating the expression
BasicTypeKind TypeChecker::visitExpr(Expr *E) {
  auto T = VisitorBase::visitExpr<BasicTypeKind>(E);
  TheTable->setExprType(E, T);
  return T;
}

void TypeChecker::visitFuncDef(FuncDef *FD) {
  assert(TheTable);
  setFuncDef(FD);
  setLocalTable(TheTable->getLocalTable(FD));
  ChildrenVisitor::visitFuncDef(FD);
}