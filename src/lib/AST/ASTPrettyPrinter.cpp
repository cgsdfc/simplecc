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

#include "simplecc/AST/ASTPrettyPrinter.h"

using namespace simplecc;

void ASTPrettyPrinter::visitAST(AST *A) {
  OS << A->getClassName() << "(";
  VisitorBase::visitAST(A);
  OS << ")";
}

/// Program(Filename=..., [
///   ConstDecl(Int, Name, NumExpr(4)),
///   VarDecl(Int, Name, true, 4),
///   FuncDef(Void, main, Args(Int A1, Char A2),
///     ConstDecl(Int, Name, NumExpr(1)),
///     VarDecl(Int, Var, false, 0),
///     AssignStmt(
///       LHS=NameExpr(Var, Store),
///       RHS=NameExpr(Name, Load),
///     ),
///   ),
/// ])
void ASTPrettyPrinter::visitProgram(ProgramAST *P) {
  OS << "Filename='" << P->getFilename() << "', [\n";
  increaseIndentLevel();
  for (DeclAST *D : P->getDecls()) {
    printIndent();
    visitAST(D);
    OS << ",\n";
  }
  decreaseIndentLevel();
  printIndent();
  OS << "]";
}

/// ConstDecl(Int, Name, NumExpr(1))
void ASTPrettyPrinter::visitConstDecl(ConstDecl *CD) {
  OS << CD->getType() << ", " << CD->getName() << ", ";
  visitAST(CD->getValue());
}

/// VarDecl(Int, Name, true, 10)
/// VarDecl(Int, Name, false, 0)
void ASTPrettyPrinter::visitVarDecl(VarDecl *VD) {
  OS << VD->getType() << ", " << VD->getName()
     << ", " << std::boolalpha << VD->isArray() << ", " << VD->getSize();
}

/// NameExpr(Name, Load)
void ASTPrettyPrinter::visitName(NameExpr *N) {
  OS << N->getName() << ", " << N->getContext();
}

/// StrExpr("string")
void ASTPrettyPrinter::visitStr(StrExpr *S) {
  OS << S->getStr();
}

/// NumExpr(1)
void ASTPrettyPrinter::visitNum(NumExpr *N) {
  OS << N->getNum();
}

/// CharExpr('a')
void ASTPrettyPrinter::visitChar(CharExpr *C) {
  OS << "'" << char(C->getChar()) << "'";
}

/// BinOpExpr(Add,
///   LHS=CharExpr('a'),
///   RHS=NumExpr(1),
/// )
void ASTPrettyPrinter::visitBinOp(BinOpExpr *B) {
  OS << B->getOp() << ",\n";
  increaseIndentLevel();

  // print out lhs.
  printIndent();
  OS << "LHS=";
  visitAST(B->getLeft());
  OS << ",\n";

  // print out rhs.
  printIndent();
  OS << "RHS=";
  visitAST(B->getRight());
  OS << ",\n";

  decreaseIndentLevel();
  printIndent();
}

/// UnaryOpExpr(UAdd, CharExpr('a'))
/// UnaryOpExpr(USub,
///   BinOpExpr(
///     CharExpr('a'),
///     NumExpr(1)
///   )
/// )
void ASTPrettyPrinter::visitUnaryOp(UnaryOpExpr *U) {
  OS << U->getOp();
  // short case.
  if (isAtomicExpr(U->getOperand())) {
    OS << ", ";
    visitAST(U->getOperand());
    OS << ")";
    return;
  }
  // long case.
  OS << ",\n";
  increaseIndentLevel();
  printIndent();
  visitAST(U->getOperand());
  OS << "\n";
  decreaseIndentLevel();
  printIndent();
}

/// ParenExpr(CharExpr('a'))
/// ParenExpr(
///   BinOpExpr(
///     CharExpr('a'),
///     NumExpr(1)
///   )
/// )
void ASTPrettyPrinter::visitParenExpr(ParenExpr *P) {
  if (isAtomicExpr(P->getValue())) {
    visitAST(P->getValue());
    return;
  }
  OS << "\n";
  increaseIndentLevel();
  printIndent();
  visitAST(P->getValue());
  OS << "\n";
  decreaseIndentLevel();
  printIndent();
}

/// BoolOpExpr(false, NumExpr(1))
/// BoolOpExpr(true,
///  BinOpExpr(GtE,
///  ...
///  )
/// )
void ASTPrettyPrinter::visitBoolOp(BoolOpExpr *B) {
  OS << std::boolalpha << bool(B->hasCompareOp());
  if (isAtomicExpr(B->getValue())) {
    OS << ", ";
    visitAST(B->getValue());
    return;
  }
  OS << ",\n";
  increaseIndentLevel();
  printIndent();
  visitAST(B->getValue());
  OS << "\n";
  decreaseIndentLevel();
  printIndent();
}

/// CallExpr(Name, Args=[])
/// CallExpr(Name, Args=[NumExpr(1)])
/// CallExpr(Name, Args=[
///   NameExpr(Name, Load),
///   CharExpr('a'),
/// ])
void ASTPrettyPrinter::visitCall(CallExpr *C) {
  OS << C->getCallee() << ", Args=";
  // Case-1: no args.
  if (hasNoArgument(C)) {
    OS << "[]";
    return;
  }
  // Case-2: only one atomic ExprAST.
  if (C->getArgs().size() == 1 && isAtomicExpr(C->getArgs().front())) {
    OS << "[";
    visitAST(C->getArgs().front());
    OS << "]";
    return;
  }
  OS << "[\n";
  increaseIndentLevel();
  for (ExprAST *E : C->getArgs()) {
    printIndent();
    visitAST(E);
    OS << ",\n";
  }
  decreaseIndentLevel();
  printIndent();
  OS << "]";
}

/// ExprStmt(CallExpr(Name, Args=[]))
/// ExprStmt(
///   CallExpr(Name, Args=[
///     NameExpr(Name, Load),
///     CharExpr('a'),
///   ])
/// )
void ASTPrettyPrinter::visitExprStmt(ExprStmt *E) {
  auto Call = static_cast<CallExpr *>(E->getValue());
  if (hasNoArgument(Call)) {
    visitAST(Call);
    return;
  }
  OS << "\n";
  increaseIndentLevel();
  printIndent();
  visitAST(Call);
  OS << "\n";
  decreaseIndentLevel();
  printIndent();
}

/// IfStmt(
///   Test=CharExpr('a'),
///   Then=[
///     WriteStmt(NumExpr(1)),
///   ],
///   Else=[
///     WriteStmt(NumExpr(2)),
///  ]
/// )
void ASTPrettyPrinter::visitIf(IfStmt *I) {
  OS << "\n";
  increaseIndentLevel();

  printIndent();
  OS << "Test=";
  visitAST(I->getCondition());
  OS << ",\n";

  printIndent();
  OS << "Then=";
  printStmtList(I->getThen());
  OS << ",\n";

  printIndent();
  OS << "Else=";
  printStmtList(I->getElse());
  OS << ",\n";

  decreaseIndentLevel();
  printIndent();
}

/// empty: []
/// only one: [WriteStmt()]
/// [
///   WriteStmt(),
///   ReadStmt(),
/// ]
void ASTPrettyPrinter::printStmtList(const std::vector<StmtAST *> &StmtList) {
  if (StmtList.empty()) {
    OS << "[]";
    return;
  }

  if (StmtList.size() == 1) {
    OS << "[";
    visitAST(StmtList.front());
    OS << "]";
    return;
  }

  OS << "[\n";
  increaseIndentLevel();
  for (StmtAST *S : StmtList) {
    printIndent();
    visitAST(S);
    OS << ",\n";
  }
  decreaseIndentLevel();
  printIndent();
  OS << "]";
}

/// ReadStmt([NameExpr(Name, Store)])
/// ReadStmt([
///  NameExpr(Name, Store),
///  NameExpr(Name, Store),
/// ])
void ASTPrettyPrinter::visitRead(ReadStmt *RD) {
  OS << "[";
  if (RD->getNames().size() == 1) {
    visitAST(RD->getNames()[0]);
    OS << "]";
    return;
  }
  OS << "\n";
  increaseIndentLevel();
  for (ExprAST *E : RD->getNames()) {
    printIndent();
    visitAST(E);
    OS << ",\n";
  }
  decreaseIndentLevel();
  printIndent();
  OS << "]";
}

/// WriteStmt(StrExpr("a"))
/// WriteStmt(NumExpr(1)))
/// WriteStmt(
///   StrExpr("a"),
///   NumExpr(1),
/// )
void ASTPrettyPrinter::visitWrite(WriteStmt *W) {
  if (!W->getValue()) {
    // No value.
    visitAST(W->getStr());
    return;
  }
  if (!W->getStr() && isAtomicExpr(W->getValue())) {
    // Only atomic ExprAST.
    visitAST(W->getValue());
    return;
  }

  OS << "\n";
  increaseIndentLevel();
  if (W->getStr()) {
    printIndent();
    visitAST(W->getStr());
    OS << ",\n";
  }
  if (W->getValue()) {
    printIndent();
    visitAST(W->getValue());
    OS << ",\n";
  }
  decreaseIndentLevel();
  printIndent();
}

/// ReturnStmt()
/// ReturnStmt(NumExpr(1))
/// ReturnStmt(
///  BinOpExpr(...)
/// )
void ASTPrettyPrinter::visitReturn(ReturnStmt *R) {
  if (!R->hasValue()) {
    return;
  }
  if (isAtomicExpr(R->getValue())) {
    visitAST(R->getValue());
    return;
  }
  OS << "\n";
  increaseIndentLevel();
  printIndent();
  visitAST(R->getValue());
  OS << "\n";
  decreaseIndentLevel();
  printIndent();
}

/// AssignStmt(
///   LHS=NameExpr(...),
///   RHS=NumExpr(...),
/// )
void ASTPrettyPrinter::visitAssign(AssignStmt *A) {
  OS << "\n";
  increaseIndentLevel();

  printIndent();
  OS << "LHS=";
  visitAST(A->getTarget());
  OS << ",\n";

  printIndent();
  OS << "RHS=";
  visitAST(A->getValue());
  OS << ",\n";
  decreaseIndentLevel();
  printIndent();
}

/// ForStmt(
///   initial=AssignStmt(...),
///   condition=BoolOpExpr(...),
///   step=AssignStmt(...),
///   body=[...],
/// )
void ASTPrettyPrinter::visitFor(ForStmt *F) {
  OS << "\n";
  increaseIndentLevel();

  // print initial.
  printIndent();
  OS << "initial=";
  visitAST(F->getInitial());
  OS << ",\n";

  // print condition.
  printIndent();
  OS << "condition=";
  visitAST(F->getCondition());
  OS << ",\n";

  // print step.
  printIndent();
  OS << "step=";
  visitAST(F->getStep());
  OS << ",\n";

  // print body.
  printIndent();
  OS << "body=";
  printStmtList(F->getBody());
  OS << ",\n";

  decreaseIndentLevel();
  printIndent();
}

/// WhileStmt(
///   condition=BoolOpExpr(...),
///   body=[...],
/// )
void ASTPrettyPrinter::visitWhile(WhileStmt *W) {
  OS << "\n";
  increaseIndentLevel();

  printIndent();
  OS << "condition=";
  visitAST(W->getCondition());
  OS << ",\n";

  printIndent();
  OS << "body=";
  printStmtList(W->getBody());
  OS << ",\n";

  decreaseIndentLevel();
  printIndent();
}

/// SubscriptExpr(Store,
///   array=Name,
///   index=NumExpr(...),
/// )
void ASTPrettyPrinter::visitSubscript(SubscriptExpr *SB) {
  OS << SB->getContext() << ",\n";
  increaseIndentLevel();

  printIndent();
  OS << "array=" << SB->getArrayName() << ",\n";

  printIndent();
  OS << "index=";
  visitAST(SB->getIndex());
  OS << ",\n";

  decreaseIndentLevel();
  printIndent();
}

// TODO: Change BasicTypeKind::Character to Char.

/// FuncDef(Void, main, Args(), [
///   ConstDecl(Int, Name, NumExpr(1)),
///   VarDecl(Int, Var, false, 0),
///   WriteStmt(...),
/// ])
/// FuncDef(Int, Fun, Args(Int Arg), [
///   ReturnStmt(NumExpr(1)),
/// ])
///
/// FuncDef(Void, Fun, Args(), [])
/// FuncDef(Void, Fun, Args(), [ReturnStmt])
void ASTPrettyPrinter::visitFuncDef(FuncDef *FD) {
  // print return type, name and arguments.
  OS << FD->getReturnType() << ", " << FD->getName() << ", ";
  printArgs(FD->getArgs());

  OS << ", [";
  // Special case when the body is empty.
  if (FD->getDecls().empty() && FD->getStmts().empty()) {
    OS << "])";
    return;
  }

  OS << "\n";
  increaseIndentLevel();

  // print declarations.
  for (DeclAST *D : FD->getDecls()) {
    printIndent();
    visitAST(D);
    OS << ",\n";
  }

  // print statements.
  for (StmtAST *S : FD->getStmts()) {
    printIndent();
    visitAST(S);
    OS << ",\n";
  }

  decreaseIndentLevel();
  printIndent();
  OS << "]";
}

/// Int Name
void ASTPrettyPrinter::visitArgDecl(ArgDecl *AD) {
  OS << AD->getType() << " " << AD->getName();
}

/// Args()
/// Args(Int A)
void ASTPrettyPrinter::printArgs(const std::vector<ArgDecl *> &Args) {
  OS << "Args";
  if (Args.empty()) {
    OS << "()";
    return;
  }
  OS << "(";
  for (auto I = Args.begin(), E = Args.end(); I != E; ++I) {
    visitAST(*I);
    if (I != E - 1)
      OS << ", ";
  }
  OS << ")";
}

void ASTPrettyPrinter::PrettyPrint(const AST *A) {
  visitAST(const_cast<AST *>(A));
}

// XXX: make it a method of ExprAST?

bool ASTPrettyPrinter::isAtomicExpr(ExprAST *E) const {
  switch (E->getKind()) {
  case ExprAST::NameExprKind:
  case ExprAST::StrExprKind:
  case ExprAST::NumExprKind:
  case ExprAST::CharExprKind:return true;
  default:return false;
  }
}

namespace simplecc {
void PrettyPrintAST(const AST &A, std::ostream &O) {
  ASTPrettyPrinter(O).PrettyPrint(&A);
  O << "\n";
}
} // namespace simplecc