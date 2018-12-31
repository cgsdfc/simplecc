#include <simplecc/Analysis/AstPrettyPrinter.h>

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
void AstPrettyPrinter::visitProgram(Program *P) {
  OS << P->getClassName() << "(";
  OS << "Filename='" << P->getFilename() << "', [\n";
  increaseIndentLevel();
  for (Decl *D : P->getDecls()) {
    printIndent();
    visitDecl(D);
    OS << ",\n";
  }
  decreaseIndentLevel();
  printIndent();
  OS << "])";
}

void AstPrettyPrinter::visitConstDecl(ConstDecl *CD) {
  OS << CD->getClassName() << "(" << CD->getType() << ", " <<
     CD->getName() << ", ";
  visitExpr(CD->getValue());
  OS << ")";
}

/// VarDecl(Int, Name, true, 10)
void AstPrettyPrinter::visitVarDecl(VarDecl *VD) {
  OS << VD->getClassName() << "(" << VD->getType() << ", " <<
     VD->getName() << ", " <<
     std::boolalpha << bool(VD->getIsArray()) << ", " <<
     VD->getSize() << ")";
}

/// NameExpr(Name, Load)
void AstPrettyPrinter::visitName(NameExpr *N) {
  OS << N->getClassName() << "(" << N->getId() << ", " << N->getCtx() << ")";
}

/// StrExpr("string")
void AstPrettyPrinter::visitStr(StrExpr *S) {
  OS << S->getClassName() << "(" << S->getS() << ")";
}

/// NumExpr(1)
void AstPrettyPrinter::visitNum(NumExpr *N) {
  OS << N->getClassName() << "(" << N->getN() << ")";
}

/// CharExpr('a')
void AstPrettyPrinter::visitChar(CharExpr *C) {
  OS << C->getClassName() << "('" << char(C->getC()) << "')";
}

/// BinOpExpr(Add,
///   LHS=CharExpr('a'),
///   RHS=NumExpr(1),
/// )
void AstPrettyPrinter::visitBinOp(BinOpExpr *B) {
  OS << B->getClassName() << "(" << B->getOp() << ",\n";

  increaseIndentLevel();
  printIndent();
  OS << "LHS=";
  visitExpr(B->getLeft());
  OS << ",\n";

  printIndent();
  OS << "RHS=";
  visitExpr(B->getRight());
  OS << ",\n";
  decreaseIndentLevel();

  printIndent();
  OS << ")";
}

void AstPrettyPrinter::printIndent() {
  for (unsigned I = 0, E = getIndentLevel(); I < E; I++) {
    OS << "  ";
  }
}

/// UnaryOpExpr(UAdd, CharExpr('a'))
/// UnaryOpExpr(USub,
///   BinOpExpr(
///     CharExpr('a'),
///     NumExpr(1)
///   )
/// )
void AstPrettyPrinter::visitUnaryOp(UnaryOpExpr *U) {
  OS << U->getClassName() << "(" << U->getOp();
  if (isAtomicExpr(U->getOperand())) {
    OS << ", ";
    visitExpr(U->getOperand());
    OS << ")";
    return;
  }
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
void AstPrettyPrinter::visitParenExpr(ParenExpr *P) {
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
void AstPrettyPrinter::visitBoolOp(BoolOpExpr *B) {
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
void AstPrettyPrinter::visitCall(CallExpr *C) {
  OS << C->getClassName() << "(" << C->getFunc() << ", Args=";
  // Case-1: no args.
  if (hasNoArgument(C)) {
    OS << "[])";
    return;
  }
  // Case-2: only one atomic Expr.
  if (C->getArgs().size() == 1 && isAtomicExpr(C->getArgs().front())) {
    OS << "[";
    visitExpr(C->getArgs().front());
    OS << "])";
    return;
  }
  OS << "[\n";
  increaseIndentLevel();
  for (Expr *E : C->getArgs()) {
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
void AstPrettyPrinter::visitExprStmt(ExprStmt *E) {
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
void AstPrettyPrinter::visitIf(IfStmt *I) {
  OS << I->getClassName() << "(\n";
  increaseIndentLevel();

  printIndent();
  OS << "Test=";
  visitExpr(I->getTest());
  OS << ",\n";

  printIndent();
  OS << "Then=";
  printStmtList(I->getBody());
  OS << ",\n";

  printIndent();
  OS << "Else=";
  printStmtList(I->getOrelse());
  OS << ",\n";

  decreaseIndentLevel();
  printIndent();
  OS << ")";
}

/// empty: []
/// [
///   WriteStmt(),
///   ReadStmt(),
/// ]
void AstPrettyPrinter::printStmtList(const std::vector<Stmt *> &StmtList) {
  if (StmtList.empty()) {
    OS << "[]";
    return;
  }
  OS << "[\n";
  increaseIndentLevel();
  for (Stmt *S : StmtList) {
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
void AstPrettyPrinter::visitRead(ReadStmt *RD) {
  OS << RD->getClassName() << "([";
  if (RD->getNames().size() == 1) {
    visitExpr(RD->getNames()[0]);
    OS << "])";
    return;
  }
  OS << "\n";
  increaseIndentLevel();
  for (Expr *E : RD->getNames()) {
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
void AstPrettyPrinter::visitWrite(WriteStmt *WR) {
  OS << WR->getClassName() << "(";
  if (!WR->getValue()) {
    // No value.
    visitExpr(WR->getStr());
    OS << ")";
    return;
  }
  if (!WR->getStr() && isAtomicExpr(WR->getValue())) {
    // Only atomic Expr.
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
void AstPrettyPrinter::visitReturn(ReturnStmt *R) {
  OS << R->getClassName();
  if (!R->getValue()) {
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
void AstPrettyPrinter::visitAssign(AssignStmt *A) {
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
void AstPrettyPrinter::visitFor(ForStmt *F) {
  OS << F->getClassName() << "(\n";
  increaseIndentLevel();

  printIndent();
  OS << "initial=";
  visitStmt(F->getInitial());
  OS << ",\n";

  printIndent();
  OS << "condition=";
  visitExpr(F->getCondition());
  OS << ",\n";

  printIndent();
  OS << "step=";
  visitStmt(F->getStep());
  OS << ",\n";

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
void AstPrettyPrinter::visitWhile(WhileStmt *W) {
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
void AstPrettyPrinter::visitSubscript(SubscriptExpr *SB) {
  OS << SB->getClassName() << "(\n";
  increaseIndentLevel();

  printIndent();
  OS << "array=" << SB->getName() << ",\n";

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
/// FuncDef(Void, Fun, Args(), [])
/// FuncDef(Void, Fun, Args(), [ReturnStmt])
void AstPrettyPrinter::visitFuncDef(FuncDef *FD) {
  OS << FD->getClassName() << "(" << FD->getReturnType() << ", " << FD->getName() << ", ";
  printArgs(FD->getArgs());
  OS << ", [";
  // Special case when the body is empty.
  if (FD->getDecls().empty() && FD->getStmts().empty()) {
    OS << "])";
    return;
  }
  OS << "\n";
  increaseIndentLevel();
  for (Decl *D : FD->getDecls()) {
    printIndent();
    visitDecl(D);
    OS << ",\n";
  }
  for (Stmt *S : FD->getStmts()) {
    printIndent();
    visitStmt(S);
    OS << ",\n";
  }
  decreaseIndentLevel();
  printIndent();
  OS << "])";
}

void AstPrettyPrinter::visitArgDecl(ArgDecl *AD) {
  OS << AD->getType() << " " << AD->getName();
}

/// Args()
/// Args(Int A)
void AstPrettyPrinter::printArgs(const std::vector<Decl *> &Args) {
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

void AstPrettyPrinter::PrettyPrint(const AST *A) {
  visitAST(const_cast<AST *>(A));
}

namespace simplecc {
void PrettyPrintAST(const AST &A, std::ostream &O) {
  AstPrettyPrinter(O).PrettyPrint(&A);
}
}