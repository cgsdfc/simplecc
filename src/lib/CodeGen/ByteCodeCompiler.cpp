#include "simplecc/CodeGen/ByteCodeCompiler.h"
#include "simplecc/CodeGen/ByteCodeFunction.h"
#include "simplecc/CodeGen/ByteCodeModule.h"

using namespace simplecc;

void ByteCodeCompiler::visitStmt(StmtAST *S) {
  Builder.setLocation(S->getLocation());
  ChildrenVisitor::visitStmt(S);
}

void ByteCodeCompiler::visitArgDecl(ArgDecl *A) {
  auto TheFunction = Builder.getInsertPoint();
  TheFunction->getFormalArguments().push_back(TheLocalTable[A->getName()]);
}

void ByteCodeCompiler::visitVarDecl(VarDecl *VD) {
  auto TheFunction = Builder.getInsertPoint();
  TheFunction->getLocalVariables().push_back(TheLocalTable[VD->getName()]);
}

void ByteCodeCompiler::visitRead(ReadStmt *RD) {
  for (auto E : RD->getNames()) {
    auto N = static_cast<NameExpr *>(E);
    const auto &Entry = TheLocalTable[N->getId()];
    Builder.CreateRead(Entry.AsVariable().getType());
    Builder.CreateStore(Entry.getScope(), Entry.getName());
  }
}

void ByteCodeCompiler::visitWrite(WriteStmt *WR) {
  if (WR->getStr()) {
    visitExpr(WR->getStr());
    Builder.CreatePrintString();
  }
  if (WR->getValue()) {
    visitExpr(WR->getValue());
    Builder.CreatePrint(TheTable->getExprType(WR->getValue()));
  }
  Builder.CreatePrintNewline();
}

void ByteCodeCompiler::visitFor(ForStmt *node) {
  visitStmt(node->getInitial());
  unsigned JumpToStart = Builder.CreateJumpForward();

  unsigned Loop = Builder.getSize();
  visitStmt(node->getStep());
  unsigned JumpToEnd =
      CompileBoolOp(static_cast<BoolOpExpr *>(node->getCondition()));

  unsigned Start = Builder.getSize();
  for (auto s : node->getBody()) {
    visitStmt(s);
  }
  unsigned JumpToLoop = Builder.CreateJumpForward();
  unsigned End = Builder.getSize();

  Builder.setJumpTargetAt(JumpToStart, Start);
  Builder.setJumpTargetAt(JumpToEnd, End);
  Builder.setJumpTargetAt(JumpToLoop, Loop);
}

void ByteCodeCompiler::visitWhile(WhileStmt *W) {
  unsigned Loop = Builder.getSize();
  unsigned JumpToEnd =
      CompileBoolOp(static_cast<BoolOpExpr *>(W->getCondition()));

  for (auto s : W->getBody()) {
    visitStmt(s);
  }
  unsigned JumpToLoop = Builder.CreateJumpForward();

  unsigned End = Builder.getSize();
  Builder.setJumpTargetAt(JumpToEnd, End);
  Builder.setJumpTargetAt(JumpToLoop, Loop);
}

void ByteCodeCompiler::visitIf(IfStmt *I) {
  unsigned JumpToElse = CompileBoolOp(static_cast<BoolOpExpr *>(I->getTest()));
  for (auto s : I->getBody()) {
    visitStmt(s);
  }

  if (I->getOrelse().empty()) {
    unsigned End = Builder.getSize();
    Builder.setJumpTargetAt(JumpToElse, End);
    return;
  }
  unsigned JumpToEnd = Builder.CreateJumpForward();

  unsigned Else = Builder.getSize();
  for (auto s : I->getOrelse()) {
    visitStmt(s);
  }
  unsigned End = Builder.getSize();

  Builder.setJumpTargetAt(JumpToElse, Else);
  Builder.setJumpTargetAt(JumpToEnd, End);
}

void ByteCodeCompiler::visitReturn(ReturnStmt *R) {
  ChildrenVisitor::visitReturn(R);
  R->getValue() ? Builder.CreateReturnValue() : Builder.CreateReturnNone();
}

unsigned ByteCodeCompiler::CompileBoolOp(BoolOpExpr *B) {
  if (B->hasCompareOp()) {
    auto BO = static_cast<BinOpExpr *>(B->getValue());
    visitExpr(BO->getLeft());
    visitExpr(BO->getRight());
    return Builder.CreateJump(BO->getOp());
  }
  ChildrenVisitor::visitBoolOp(B);
  return Builder.CreateJumpIfFalse();
}

void ByteCodeCompiler::visitCall(CallExpr *C) {
  /// Visit all the args first.
  ChildrenVisitor::visitCall(C);
  Builder.CreateCallFunction(C->getFunc(), C->getArgs().size());
}

void ByteCodeCompiler::visitStr(simplecc::StrExpr *S) {
  Builder.CreateLoadString(TheModule->getStringLiteralID(S->getS()));
}

void ByteCodeCompiler::visitSubscript(SubscriptExpr *SB) {
  const auto &Entry = TheLocalTable[SB->getName()];
  // load array
  Builder.CreateLoad(Entry.getScope(), SB->getName());
  // calculate index
  visitExpr(SB->getIndex());
  // do subscript
  Builder.CreateSubscr(SB->getContext());
}

void ByteCodeCompiler::visitName(NameExpr *N) {
  const auto &Entry = TheLocalTable[N->getId()];
  if (Entry.IsConstant()) {
    Builder.CreateLoadConst(Entry.AsConstant().getValue());
    return;
  }
  N->getContext() == ExprContextKind::Load
      ? Builder.CreateLoad(Entry.getScope(), N->getId())
      : Builder.CreateStore(Entry.getScope(), N->getId());
}

void ByteCodeCompiler::visitFuncDef(FuncDef *FD) {
  /// Create the function and set members.
  auto TheFunction = ByteCodeFunction::Create(TheModule);
  TheFunction->setName(FD->getName());
  setLocalTable(TheTable->getLocalTable(FD));
  TheFunction->setLocalTable(TheLocalTable);

  /// Emit code for the function body.
  Builder.setInsertPoint(TheFunction);
  ChildrenVisitor::visitFuncDef(FD);

  // It is important we always return since we don't
  // construct any BasicBlock to detect improper returns.
  Builder.CreateReturnNone();
}

void ByteCodeCompiler::visitProgram(ProgramAST *P) {
  for (DeclAST *D : P->getDecls()) {
    switch (D->getKind()) {
    case DeclAST::FuncDefKind:
      visitFuncDef(static_cast<FuncDef *>(D));
      break;
    case DeclAST::VarDeclKind:
      // Collect global objects.
      TheModule->getGlobalVariables().push_back(
          TheTable->getGlobalEntry(D->getName()));
      break;
    default:
      break; // Ignore ConstDecl.
    }
  }
}

// public interface
void ByteCodeCompiler::Compile(ProgramAST *P, const SymbolTable &S,
                               ByteCodeModule &M) {
  EM.clear();
  M.clear();
  setTable(&S);
  setModule(&M);
  visitProgram(P);
}