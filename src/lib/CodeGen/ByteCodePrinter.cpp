#include "simplecc/CodeGen/ByteCodePrinter.h"

using namespace simplecc;

void ByteCodePrinter::visitConstDecl(ConstDecl *CD) {
  auto T = CStringFromBasicTypeKind(CD->getType());
  ExprValue val = visitExpr(CD->getValue());
  w.WriteLine("const", T, CD->getName(), "=", val);
}

void ByteCodePrinter::visitVarDecl(VarDecl *VD) {
  auto T = CStringFromBasicTypeKind(VD->getType());
  if (!VD->isArray()) {
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

void ByteCodePrinter::visitRead(ReadStmt *RD) {
  for (auto name : RD->getNames()) {
    w.WriteLine("scanf", visitExpr(name));
  }
}

void ByteCodePrinter::visitWrite(WriteStmt *WR) {
  if (WR->getStr()) {
    w.WriteLine("printf", visitExpr(WR->getStr()));
  }
  if (WR->getValue()) {
    w.WriteLine("printf", visitExpr(WR->getValue()));
  }
}

void ByteCodePrinter::visitAssign(AssignStmt *A) {
  ExprValue RHS = visitExpr(A->getValue());
  if (IsInstance<NameExpr>(A->getTarget())) {
    ExprValue LHS = visitExpr(A->getTarget());
    w.WriteLine(LHS, "=", RHS);
    return;
  }

  auto SB = subclass_cast<SubscriptExpr>(A->getTarget());
  assert(SB);
  ExprValue Idx = visitExpr(SB->getIndex());
  w.getOuts() << SB->getArrayName() << "[" << Idx << "] = " << RHS << "\n";
}

void ByteCodePrinter::visitReturn(ReturnStmt *R) {
  if (R->hasValue()) {
    w.WriteLine("ret", visitExpr(R->getValue()));
    return;
  }
  w.WriteLine("ret");
}

ExprValue ByteCodePrinter::visitCall(CallExpr *C) {
  for (auto A : C->getArgs()) {
    w.WriteLine("push", visitExpr(A));
  }
  w.WriteLine("call", C->getCallee());
  /// return a value even if void function
  ExprValue Ret = MakeTemporary();
  w.WriteLine(Ret, "= RET");
  return Ret;
}

LineLabel ByteCodePrinter::CompileBoolOp(BoolOpExpr *B) {
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

void ByteCodePrinter::visitFor(ForStmt *F) {
  LineLabel Start = MakeLineLabel();
  visitStmt(F->getInitial());
  w.WriteLine("GOTO", Start.Inline(true));

  LineLabel Loop = MakeLineLabel();
  w.WriteLine(Loop.Inline(false));
  visitStmt(F->getStep());

  LineLabel End = CompileBoolOp(static_cast<BoolOpExpr *>(F->getCondition()));
  w.WriteLine(Start.Inline(false));

  for (StmtAST *S : F->getBody()) {
    visitStmt(S);
  }

  w.WriteLine("GOTO", Loop.Inline(true));
  w.WriteLine(End.Inline(false));
}

void ByteCodePrinter::visitIf(IfStmt *I) {
  LineLabel Else = CompileBoolOp(static_cast<BoolOpExpr *>(I->getCondition()));

  for (auto S : I->getThen()) {
    visitStmt(S);
  }

  if (I->getElse().empty()) {
    w.WriteLine(Else.Inline(false));
    return;
  }

  LineLabel End = MakeLineLabel();
  w.WriteLine("GOTO", End.Inline(true));
  w.WriteLine(Else.Inline(false));

  for (auto S : I->getElse()) {
    visitStmt(S);
  }
  w.WriteLine(End.Inline(false));
}

void ByteCodePrinter::visitWhile(WhileStmt *W) {
  LineLabel Loop = MakeLineLabel();
  w.WriteLine(Loop.Inline(false));
  LineLabel End = CompileBoolOp(static_cast<BoolOpExpr *>(W->getCondition()));

  for (auto S : W->getBody()) {
    visitStmt(S);
  }

  w.WriteLine("GOTO", Loop.Inline(true));
  w.WriteLine(End.Inline(false));
}

ExprValue ByteCodePrinter::visitBinOp(BinOpExpr *B) {
  auto Op = CStringFromOperatorKind(B->getOp());
  ExprValue L = visitExpr(B->getLeft());
  ExprValue R = visitExpr(B->getRight());
  ExprValue Result = MakeTemporary();
  w.WriteLine(Result, "=", L, Op, R);
  return Result;
}

ExprValue ByteCodePrinter::visitUnaryOp(UnaryOpExpr *U) {
  auto Op = CStringFromUnaryopKind(U->getOp());
  ExprValue Operand = visitExpr(U->getOperand());
  ExprValue Result = MakeTemporary();
  w.WriteLine(Result, "=", Op, Operand);
  return Result;
}

ExprValue ByteCodePrinter::visitSubscript(SubscriptExpr *SB) {
  assert(SB->getContext() == ExprContextKind::Load &&
         "Store must be handle by visitAssign()");
  ExprValue Idx = visitExpr(SB->getIndex());
  ExprValue Result = MakeTemporary();
  w.getOuts() << Result << " = " << SB->getArrayName() << "[" << Idx << "]\n";
  return Result;
}

/// Sanity check
bool ExprValue::Check() const {
  if (!Factor) {
    return Temporary >= 0;
  } else {
    return (IsInstance<CharExpr>(Factor) || IsInstance<NumExpr>(Factor) ||
            IsInstance<NameExpr>(Factor) || IsInstance<StrExpr>(Factor));
  }
}

void ExprValue::Format(std::ostream &O) const {
  if (!Factor) {
    O << "t" << Temporary;
    return;
  }
  switch (Factor->getKind()) {
  case ExprAST::CharExprKind:
    O << "'" << char(static_cast<CharExpr *>(Factor)->getChar()) << "'";
    break;
  case ExprAST::NumExprKind:
    O << static_cast<NumExpr *>(Factor)->getNum();
    break;
  case ExprAST::NameExprKind:
    O << static_cast<NameExpr *>(Factor)->getName();
    break;
  case ExprAST::StrExprKind:
    O << static_cast<StrExpr *>(Factor)->getStr();
    break;
  default:
    assert(false && "Unhandled Factor ExprAST");
  }
}

ExprValue::ExprValue(int Temp) : Factor(nullptr), Temporary(Temp) {
  assert(Check());
}

ExprValue::ExprValue(ExprAST *Factor) : Factor(Factor), Temporary(-1) {
  assert(Check());
}

void LineLabel::Format(std::ostream &O) const {
  O << "Label_" << No;
  if (!IsInline)
    O << ":";
}