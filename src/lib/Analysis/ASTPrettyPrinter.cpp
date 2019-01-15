#include "simplecc/Analysis/ASTPrettyPrinter.h"

using namespace simplecc;

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
  OS << P->getClassName() << "(";
  OS << "Filename='" << P->getFilename() << "', [\n";
  increaseIndentLevel();
  for (DeclAST *D : P->getDecls()) {
    printIndent();
    visitDecl(D);
    OS << ",\n";
  }
  decreaseIndentLevel();
  printIndent();
  OS << "])";
}

/// ConstDecl(Int, Name, NumExpr(1))
void ASTPrettyPrinter::visitConstDecl(ConstDecl *CD) {
  OS << CD->getClassName() << "(" << CD->getType() << ", " << CD->getName()
     << ", ";
  visitExpr(CD->getValue());
  OS << ")";
}

/// VarDecl(Int, Name, true, 10)
/// VarDecl(Int, Name, false, 0)
void ASTPrettyPrinter::visitVarDecl(VarDecl *VD) {
  OS << VD->getClassName() << "(" << VD->getType() << ", " << VD->getName()
     << ", " << std::boolalpha << VD->isArray() << ", " << VD->getSize()
     << ")";
}

/// NameExpr(Name, Load)
void ASTPrettyPrinter::visitName(NameExpr *N) {
  OS << N->getClassName() << "(" << N->getName() << ", " << N->getContext()
     << ")";
}

/// StrExpr("string")
void ASTPrettyPrinter::visitStr(StrExpr *S) {
  OS << S->getClassName() << "(" << S->getStr() << ")";
}

/// NumExpr(1)
void ASTPrettyPrinter::visitNum(NumExpr *N) {
  OS << N->getClassName() << "(" << N->getNum() << ")";
}

/// CharExpr('a')
void ASTPrettyPrinter::visitChar(CharExpr *C) {
  OS << C->getClassName() << "('" << char(C->getChar()) << "')";
}

/// BinOpExpr(Add,
///   LHS=CharExpr('a'),
///   RHS=NumExpr(1),
/// )
void ASTPrettyPrinter::visitBinOp(BinOpExpr *B) {
  OS << B->getClassName() << "(" << B->getOp() << ",\n";
  increaseIndentLevel();

  // print out lhs.
  printIndent();
  OS << "LHS=";
  visitExpr(B->getLeft());
  OS << ",\n";

  // print out rhs.
  printIndent();
  OS << "RHS=";
  visitExpr(B->getRight());
  OS << ",\n";

  decreaseIndentLevel();
  printIndent();
  OS << ")";
}

/// UnaryOpExpr(UAdd, CharExpr('a'))
/// UnaryOpExpr(USub,
///   BinOpExpr(
///     CharExpr('a'),
///     NumExpr(1)
///   )
/// )
void ASTPrettyPrinter::visitUnaryOp(UnaryOpExpr *U) {
  OS << U->getClassName() << "(" << U->getOp();
  // short case.
  if (isAtomicExpr(U->getOperand())) {
    OS << ", ";
    visitExpr(U->getOperand());
    OS << ")";
    return;
  }
  // long case.
  OS << ",\n";
  increaseIndentLevel();
  printIndent();
  visitExpr(U->getOperand());
  OS << "\n";
  decreaseIndentLevel();
  printIndent();
  OS << ")";
}

/// ParenExpr(CharExpr('a'))
/// ParenExpr(
///   BinOpExpr(
///     CharExpr('a'),
///     NumExpr(1)
///   )
/// )
void ASTPrettyPrinter::visitParenExpr(ParenExpr *P) {
  OS << P->getClassName() << "(";
  if (isAtomicExpr(P->getValue())) {
    visitExpr(P->getValue());
    OS << ")";
    return;
  }
  OS << "\n";
  increaseIndentLevel();
  printIndent();
  visitExpr(P->getValue());
  OS << "\n";
  decreaseIndentLevel();
  printIndent();
  OS << ")";
}

/// BoolOpExpr(false, NumExpr(1))
/// BoolOpExpr(true,
///  BinOpExpr(GtE,
///  ...
///  )
/// )
void ASTPrettyPrinter::visitBoolOp(BoolOpExpr *B) {
  OS << B->getClassName() << "(" << std::boolalpha << bool(B->hasCompareOp());
  if (isAtomicExpr(B->getValue())) {
    OS << ", ";
    visitExpr(B->getValue());
    OS << ")";
    return;
  }
  OS << ",\n";
  increaseIndentLevel();
  printIndent();
  visitExpr(B->getValue());
  OS << "\n";
  decreaseIndentLevel();
  printIndent();
  OS << ")";
}

/// CallExpr(Name, Args=[])
/// CallExpr(Name, Args=[NumExpr(1)])
/// CallExpr(Name, Args=[
///   NameExpr(Name, Load),
///   CharExpr('a'),
/// ])
void ASTPrettyPrinter::visitCall(CallExpr *C) {
  OS << C->getClassName() << "(" << C->getCallee() << ", Args=";
  // Case-1: no args.
  if (hasNoArgument(C)) {
    OS << "[])";
    return;
  }
  // Case-2: only one atomic ExprAST.
  if (C->getArgs().size() == 1 && isAtomicExpr(C->getArgs().front())) {
    OS << "[";
    visitExpr(C->getArgs().front());
    OS << "])";
    return;
  }
  OS << "[\n";
  increaseIndentLevel();
  for (ExprAST *E : C->getArgs()) {
    printIndent();
    visitExpr(E);
    OS << ",\n";
  }
  decreaseIndentLevel();
  printIndent();
  OS << "])";
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
  OS << E->getClassName() << "(";
  if (hasNoArgument(Call)) {
    visitExpr(Call);
    OS << ")";
    return;
  }
  OS << "\n";
  increaseIndentLevel();
  printIndent();
  visitExpr(Call);
  OS << "\n";
  decreaseIndentLevel();
  printIndent();
  OS << ")";
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
  OS << I->getClassName() << "(\n";
  increaseIndentLevel();

  printIndent();
  OS << "Test=";
  visitExpr(I->getCondition());
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
  OS << ")";
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
    visitStmt(StmtList.front());
    OS << "]";
    return;
  }

  OS << "[\n";
  increaseIndentLevel();
  for (StmtAST *S : StmtList) {
    printIndent();
    visitStmt(S);
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
  OS << RD->getClassName() << "([";
  if (RD->getNames().size() == 1) {
    visitExpr(RD->getNames()[0]);
    OS << "])";
    return;
  }
  OS << "\n";
  increaseIndentLevel();
  for (ExprAST *E : RD->getNames()) {
    printIndent();
    visitExpr(E);
    OS << ",\n";
  }
  decreaseIndentLevel();
  printIndent();
  OS << "])";
}

/// WriteStmt(StrExpr("a"))
/// WriteStmt(NumExpr(1)))
/// WriteStmt(
///   StrExpr("a"),
///   NumExpr(1),
/// )
void ASTPrettyPrinter::visitWrite(WriteStmt *WR) {
  OS << WR->getClassName() << "(";
  if (!WR->getValue()) {
    // No value.
    visitExpr(WR->getStr());
    OS << ")";
    return;
  }
  if (!WR->getStr() && isAtomicExpr(WR->getValue())) {
    // Only atomic ExprAST.
    visitExpr(WR->getValue());
    OS << ")";
    return;
  }

  OS << "\n";
  increaseIndentLevel();
  if (WR->getStr()) {
    printIndent();
    visitExpr(WR->getStr());
    OS << ",\n";
  }
  if (WR->getValue()) {
    printIndent();
    visitExpr(WR->getValue());
    OS << ",\n";
  }
  decreaseIndentLevel();
  printIndent();
  OS << ")";
}

/// ReturnStmt
/// ReturnStmt(NumExpr(1))
/// ReturnStmt(
///  BinOpExpr(...)
/// )
void ASTPrettyPrinter::visitReturn(ReturnStmt *R) {
  OS << R->getClassName();
  if (!R->hasValue()) {
    return;
  }
  if (isAtomicExpr(R->getValue())) {
    OS << "(";
    visitExpr(R->getValue());
    OS << ")";
    return;
  }
  OS << "(\n";
  increaseIndentLevel();
  printIndent();
  visitExpr(R->getValue());
  OS << "\n";
  decreaseIndentLevel();
  printIndent();
  OS << ")";
}

/// AssignStmt(
///   LHS=NameExpr(...),
///   RHS=NumExpr(...),
/// )
void ASTPrettyPrinter::visitAssign(AssignStmt *A) {
  OS << A->getClassName() << "(\n";
  increaseIndentLevel();

  printIndent();
  OS << "LHS=";
  visitExpr(A->getTarget());
  OS << ",\n";

  printIndent();
  OS << "RHS=";
  visitExpr(A->getValue());
  OS << ",\n";
  decreaseIndentLevel();
  printIndent();
  OS << ")";
}

/// ForStmt(
///   initial=AssignStmt(...),
///   condition=BoolOpExpr(...),
///   step=AssignStmt(...),
///   body=[...],
/// )
void ASTPrettyPrinter::visitFor(ForStmt *F) {
  OS << F->getClassName() << "(\n";
  increaseIndentLevel();

  // print initial.
  printIndent();
  OS << "initial=";
  visitStmt(F->getInitial());
  OS << ",\n";

  // print condition.
  printIndent();
  OS << "condition=";
  visitExpr(F->getCondition());
  OS << ",\n";

  // print step.
  printIndent();
  OS << "step=";
  visitStmt(F->getStep());
  OS << ",\n";

  // print body.
  printIndent();
  OS << "body=";
  printStmtList(F->getBody());
  OS << ",\n";

  decreaseIndentLevel();
  printIndent();
  OS << ")";
}

/// WhileStmt(
///   condition=BoolOpExpr(...),
///   body=[...],
/// )
void ASTPrettyPrinter::visitWhile(WhileStmt *W) {
  OS << W->getClassName() << "(\n";
  increaseIndentLevel();

  printIndent();
  OS << "condition=";
  visitExpr(W->getCondition());
  OS << ",\n";

  printIndent();
  OS << "body=";
  printStmtList(W->getBody());
  OS << ",\n";

  decreaseIndentLevel();
  printIndent();
  OS << ")";
}

/// SubscriptExpr(
///   array=Name,
///   index=NumExpr(...),
/// )
void ASTPrettyPrinter::visitSubscript(SubscriptExpr *SB) {
  OS << SB->getClassName() << "(" << SB->getContext() << "\n";
  increaseIndentLevel();

  printIndent();
  OS << "array=" << SB->getArrayName() << ",\n";

  printIndent();
  OS << "index=";
  visitExpr(SB->getIndex());
  OS << ",\n";

  decreaseIndentLevel();
  printIndent();
  OS << ")";
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
  OS << FD->getClassName() << "(" << FD->getReturnType() << ", "
     << FD->getName() << ", ";
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
    visitDecl(D);
    OS << ",\n";
  }

  // print statements.
  for (StmtAST *S : FD->getStmts()) {
    printIndent();
    visitStmt(S);
    OS << ",\n";
  }

  decreaseIndentLevel();
  printIndent();
  OS << "])";
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
    visitDecl(*I);
    if (I != E - 1)
      OS << ", ";
  }
  OS << ")";
}

void ASTPrettyPrinter::PrettyPrint(const AST *A) {
  visitAST(const_cast<AST *>(A));
}

bool ASTPrettyPrinter::isAtomicExpr(ExprAST *E) const {
  switch (E->getKind()) {
  case ExprAST::NameExprKind:
  case ExprAST::StrExprKind:
  case ExprAST::NumExprKind:
  case ExprAST::CharExprKind:
    return true;
  default:
    return false;
  }
}

namespace simplecc {
void PrettyPrintAST(const AST &A, std::ostream &O) {
  ASTPrettyPrinter(O).PrettyPrint(&A);
  O << "\n";
}
} // namespace simplecc