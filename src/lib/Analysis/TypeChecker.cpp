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

#include "simplecc/Analysis/TypeChecker.h"

using namespace simplecc;

void TypeChecker::visitRead(ReadStmt *RD) {
  for (auto N : RD->getNames()) {
    const auto &Entry = getSymbolEntry(N->getName());
    if (!Entry.IsVariable()) {
      Error(N->getLocation(), "scanf() only applies to variables.");
      continue;
    }
  }
}

void TypeChecker::visitWrite(WriteStmt *W) {
  if (W->getValue()) {
    CheckExprOperand(W->getValue());
  }
}

void TypeChecker::visitReturn(ReturnStmt *R) {
  auto ActuallyReturn =
      R->hasValue() ? visitExpr(R->getValue()) : BasicTypeKind::Void;
  auto ShouldReturn = TheFuncDef->getReturnType();

  // order a strict match
  if (ShouldReturn != ActuallyReturn) {
    Error(R->getLocation(), TheFuncDef->getName(), "must return",
          CStringFromBasicTypeKind(ShouldReturn));
  }
}

void TypeChecker::visitAssign(AssignStmt *A) {
  int Errs = getErrorCount();
  auto RHS = CheckExprOperand(A->getValue());
  auto LHS = visitExpr(A->getTarget());

  if (IsOk(Errs) && LHS != RHS) {
    Error(A->getLocation(), "cannot assign", CStringFromBasicTypeKind(RHS),
          "to", CStringFromBasicTypeKind(LHS));
  }
}

// check the operand of BoolOpExpr, restrict to int
void TypeChecker::CheckBoolOpOperand(ExprAST *E) {
  auto Msg = "operands of condition must be all int";
  auto Errs = getErrorCount();
  auto T = visitExpr(E);
  if (IsOk(Errs) && T != BasicTypeKind::Int) {
    Error(E->getLocation(), Msg);
  }
}

BasicTypeKind TypeChecker::visitBoolOp(BoolOpExpr *B) {
  if (B->hasCompareOp()) {
    auto Bin = static_cast<BinOpExpr *>(B->getValue());
    CheckBoolOpOperand(Bin->getLeft());
    CheckBoolOpOperand(Bin->getRight());
  } else {
    CheckBoolOpOperand(B->getValue());
  }
  return BasicTypeKind::Int;
}

// check the operand of ExprAST, restrict to NOT void
BasicTypeKind TypeChecker::CheckExprOperand(ExprAST *E) {
  auto Msg = "using void value in an expression";
  auto Errs = getErrorCount();
  auto T = visitExpr(E);
  if (IsOk(Errs) && T == BasicTypeKind::Void) {
    Error(E->getLocation(), Msg);
  }
  return T;
}

BasicTypeKind TypeChecker::visitBinOp(BinOpExpr *B) {
  CheckExprOperand(B->getLeft());
  CheckExprOperand(B->getRight());
  return BasicTypeKind::Int;
}

BasicTypeKind TypeChecker::visitUnaryOp(UnaryOpExpr *U) {
  CheckExprOperand(U->getOperand());
  return BasicTypeKind::Int;
}

BasicTypeKind TypeChecker::visitParenExpr(ParenExpr *PE) {
  CheckExprOperand(PE->getValue());
  return BasicTypeKind::Int;
}

BasicTypeKind TypeChecker::visitCall(CallExpr *C) {
  const auto &Entry = getSymbolEntry(C->getCallee());
  if (!Entry.IsFunction()) {
    Error(C->getLocation(), Entry.getName(), "is not a function");
    return BasicTypeKind::Void;
  }

  auto Ty = Entry.AsFunction();
  auto NumFormal = Ty.getNumArgs();
  auto NumActual = C->getArgs().size();
  if (NumFormal != NumActual) {
    Error(C->getLocation(), C->getCallee(), "expects", NumFormal,
          "arguments, got", NumActual);
  }

  // check args
  auto Size = std::min(NumFormal, NumActual);
  for (int I = 0; I < Size; I++) {
    auto ActualTy = visitExpr(C->getArgs()[I]);
    auto FormalTy = Ty.getArgTypeAt(I);
    if (ActualTy != FormalTy) {
      Error(C->getArgs()[I]->getLocation(), "argument", I + 1, "of",
            C->getCallee(), "must be", CStringFromBasicTypeKind(FormalTy));
    }
  }
  return Ty.getReturnType();
}

BasicTypeKind TypeChecker::visitSubscript(SubscriptExpr *SB) {
  const auto &Entry = getSymbolEntry(SB->getArrayName());
  if (!Entry.IsArray()) {
    Error(SB->getLocation(), Entry.getName(), "is not an array");
    return BasicTypeKind::Void;
  }

  int Errs = getErrorCount();
  auto Idx = visitExpr(SB->getIndex());
  if (IsOk(Errs) && Idx != BasicTypeKind::Int) {
    Error(SB->getLocation(), "array index must be int");
  }

  return Entry.AsArray().getElementType();
}

static void CheckNoLoadFunction(SymbolEntry Entry, NameExpr *N) {
  assert(!(Entry.IsFunction() && N->getContext() == ExprContextKind::Load) &&
      "There should be no NameExpr being a Function!");
}

BasicTypeKind TypeChecker::visitName(NameExpr *N) {
  const auto &Entry = getSymbolEntry(N->getName());
  // Catch this frequent error first.
  CheckNoLoadFunction(Entry, N);

  if (N->getContext() == ExprContextKind::Load && Entry.IsArray()) {
    Error(N->getLocation(), "using an array in an expression");
    return BasicTypeKind::Void;
  }
  if (N->getContext() == ExprContextKind::Store && !Entry.IsVariable()) {
    Error(N->getLocation(), "only variables can be assigned to");
    return BasicTypeKind::Void;
  }
  if (Entry.IsConstant())
    return Entry.AsConstant().getType();
  return Entry.AsVariable().getType();
}

// not actually used, for instantiation only
BasicTypeKind TypeChecker::visitStr(StrExpr *) {
  assert(false && "visitStr() shall not be called");
  return BasicTypeKind::Void;
}

// Return the type of evaluating the expression
BasicTypeKind TypeChecker::visitExpr(ExprAST *E) {
  return VisitorBase::visitExpr<BasicTypeKind>(E);
}

void TypeChecker::visitFuncDef(FuncDef *FD) {
  setFuncDef(FD);
  ContextualVisitor::visitFuncDef(FD);
}

TypeChecker::TypeChecker() {
  setErrorType("TypeError");
}