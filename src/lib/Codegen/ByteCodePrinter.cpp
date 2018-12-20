#include "simplecc/Codegen/ByteCodePrinter.h"

using namespace simplecc;

void ByteCodePrinter::visitConstDecl(ConstDecl *CD) {
  auto T = CStringFromBasicTypeKind(CD->getType());
  ExprValue val = visitExpr(CD->getValue());
  w.WriteLine("const", T, CD->getName(), "=", val);
}

void ByteCodePrinter::visitVarDecl(VarDecl *VD) {
  auto T = CStringFromBasicTypeKind(VD->getType());
  if (!VD->getIsArray()) {
    w.WriteLine("var", T, VD->getName());
    return;
  }
  w.getOuts() << "var " << T << " " << VD->getName() << "[" << VD->getSize()
              << "]\n";
}

void ByteCodePrinter::visitArgDecl(ArgDecl *AD) {
  w.WriteLine("para", CStringFromBasicTypeKind(AD->getType()), AD->getName());
}

void ByteCodePrinter::visitFuncDef(FuncDef *FD) {
  w.WriteLine();
  const char *Ret = CStringFromBasicTypeKind(FD->getReturnType());
  w.getOuts() << Ret << " " << FD->getName() << "()\n";
  ChildrenVisitor::visitFuncDef(FD);
  w.WriteLine();
}

void ByteCodePrinter::visitRead(Read *RD) {
  for (auto name : RD->getNames()) {
    w.WriteLine("scanf", visitExpr(name));
  }
}

void ByteCodePrinter::visitWrite(Write *WR) {
  if (WR->getStr()) {
    w.WriteLine("printf", visitExpr(WR->getStr()));
  }
  if (WR->getValue()) {
    w.WriteLine("printf", visitExpr(WR->getValue()));
  }
}

void ByteCodePrinter::visitAssign(Assign *A) {
  ExprValue RHS = visitExpr(A->getValue());
  if (IsInstance<Name>(A->getTarget())) {
    ExprValue LHS = visitExpr(A->getTarget());
    w.WriteLine(LHS, "=", RHS);
    return;
  }

  auto SB = subclass_cast<Subscript>(A->getTarget());
  assert(SB);
  ExprValue Idx = visitExpr(SB->getIndex());
  w.getOuts() << SB->getName() << "[" << Idx << "] = " << RHS << "\n";
}

void ByteCodePrinter::visitReturn(Return *R) {
  if (R->getValue()) {
    w.WriteLine("ret", visitExpr(R->getValue()));
    return;
  }
  w.WriteLine("ret");
}

ExprValue ByteCodePrinter::visitCall(Call *C) {
  for (auto A : C->getArgs()) {
    w.WriteLine("push", visitExpr(A));
  }
  w.WriteLine("call", C->getFunc());
  /// return a value even if void function
  ExprValue Ret = MakeTemporary();
  w.WriteLine(Ret, "= RET");
  return Ret;
}

LineLabel ByteCodePrinter::CompileBoolOp(BoolOp *B) {
  auto OldTmp = getTempCounter();
  ExprValue Val = visitExpr(B->getValue());
  if (OldTmp == getTempCounter()) {
    /// TempCounter didn't increase, we need to hold the result
    w.WriteLine(MakeTemporary(), "=", Val);
  }
  LineLabel LB = MakeLineLabel();
  w.WriteLine("BZ", LB.Inline(true));
  return LB;
}

void ByteCodePrinter::visitFor(For *F) {
  LineLabel Start = MakeLineLabel();
  visitStmt(F->getInitial());
  w.WriteLine("GOTO", Start.Inline(true));

  LineLabel Loop = MakeLineLabel();
  w.WriteLine(Loop.Inline(false));
  visitStmt(F->getStep());

  LineLabel End = CompileBoolOp(static_cast<BoolOp *>(F->getCondition()));
  w.WriteLine(Start.Inline(false));

  for (Stmt *S : F->getBody()) {
    visitStmt(S);
  }

  w.WriteLine("GOTO", Loop.Inline(true));
  w.WriteLine(End.Inline(false));
}

void ByteCodePrinter::visitIf(If *I) {
  LineLabel Else = CompileBoolOp(static_cast<BoolOp *>(I->getTest()));

  for (auto S : I->getBody()) {
    visitStmt(S);
  }

  if (I->getOrelse().empty()) {
    w.WriteLine(Else.Inline(false));
    return;
  }

  LineLabel End = MakeLineLabel();
  w.WriteLine("GOTO", End.Inline(true));
  w.WriteLine(Else.Inline(false));

  for (auto S : I->getOrelse()) {
    visitStmt(S);
  }
  w.WriteLine(End.Inline(false));
}

void ByteCodePrinter::visitWhile(While *W) {
  LineLabel Loop = MakeLineLabel();
  w.WriteLine(Loop.Inline(false));
  LineLabel End = CompileBoolOp(static_cast<BoolOp *>(W->getCondition()));

  for (auto S : W->getBody()) {
    visitStmt(S);
  }

  w.WriteLine("GOTO", Loop.Inline(true));
  w.WriteLine(End.Inline(false));
}

ExprValue ByteCodePrinter::visitBinOp(BinOp *B) {
  auto Op = CStringFromOperatorKind(B->getOp());
  ExprValue L = visitExpr(B->getLeft());
  ExprValue R = visitExpr(B->getRight());
  ExprValue Result = MakeTemporary();
  w.WriteLine(Result, "=", L, Op, R);
  return Result;
}

ExprValue ByteCodePrinter::visitUnaryOp(UnaryOp *U) {
  auto Op = CStringFromUnaryopKind(U->getOp());
  ExprValue Operand = visitExpr(U->getOperand());
  ExprValue Result = MakeTemporary();
  w.WriteLine(Result, "=", Op, Operand);
  return Result;
}

ExprValue ByteCodePrinter::visitSubscript(Subscript *SB) {
  assert(SB->getCtx() == ExprContextKind::Load &&
      "Store must be handle by visitAssign()");
  ExprValue Idx = visitExpr(SB->getIndex());
  ExprValue Result = MakeTemporary();
  w.getOuts() << Result << " = " << SB->getName() << "[" << Idx << "]\n";
  return Result;
}

/// Sanity check
bool ExprValue::Check() const {
  if (!Factor) {
    return Temporary >= 0;
  } else {
    return (IsInstance<Char>(Factor) || IsInstance<Num>(Factor) ||
        IsInstance<Name>(Factor) || IsInstance<Str>(Factor));
  }
}

void ExprValue::Format(std::ostream &O) const {
  if (!Factor) {
    O << "t" << Temporary;
  } else if (auto C = subclass_cast<Char>(Factor)) {
    O << "'" << static_cast<char>(C->getC()) << "'";
  } else if (auto N = subclass_cast<Num>(Factor)) {
    O << N->getN();
  } else if (auto N = subclass_cast<Name>(Factor)) {
    O << N->getId();
  } else {
    O << static_cast<Str *>(Factor)->getS();
  }
}

void LineLabel::Format(std::ostream &O) const {
  O << "Label_" << No;
  if (!IsInlined)
    O << ":";
}