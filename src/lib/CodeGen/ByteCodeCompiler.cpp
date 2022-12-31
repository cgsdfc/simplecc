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

#include <simplecc/Analysis/TypeEvaluator.h>
#include "simplecc/CodeGen/ByteCodeCompiler.h"
#include "simplecc/CodeGen/ByteCodeModule.h"

using namespace simplecc;

void ByteCodeCompiler::visitStmt(StmtAST *S) {
  Builder.setLineno(S->getLocation());
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
  for (auto N : RD->getNames()) {
    const auto &Entry = TheLocalTable[N->getName()];
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
    auto ValTy = TypeEvaluator::getExprType(WR->getValue(), TheLocalTable);
    Builder.CreatePrint(ValTy);
  }
  /// Add a newline for ease of reading.
  Builder.CreatePrintNewline();
}

void ByteCodeCompiler::visitFor(ForStmt *F) {
  visitStmt(F->getInitial());
  unsigned JumpToStart = Builder.CreateJumpForward();

  unsigned Loop = Builder.getSize();
  visitStmt(F->getStep());
  unsigned JumpToEnd =
      CompileBoolOp(static_cast<BoolOpExpr *>(F->getCondition()));

  unsigned Start = Builder.getSize();
  for (auto s : F->getBody()) {
    visitStmt(s);
  }
  unsigned JumpToLoop = Builder.CreateJumpForward();
  unsigned End = Builder.getSize();

  /// Link offset to jump.
  Builder.setJumpTargetAt(JumpToStart, Start);
  Builder.setJumpTargetAt(JumpToEnd, End);
  Builder.setJumpTargetAt(JumpToLoop, Loop);
}

void ByteCodeCompiler::visitWhile(WhileStmt *W) {
  /// label Loop:
  unsigned Loop = Builder.getSize();
  unsigned JumpToEnd =
      CompileBoolOp(static_cast<BoolOpExpr *>(W->getCondition()));

  /// Compile body.
  for (auto s : W->getBody()) {
    visitStmt(s);
  }
  /// Jump back to Loop.
  unsigned JumpToLoop = Builder.CreateJumpForward();
  unsigned End = Builder.getSize();

  /// Link offset to jump.
  Builder.setJumpTargetAt(JumpToEnd, End);
  Builder.setJumpTargetAt(JumpToLoop, Loop);
}

void ByteCodeCompiler::visitIf(IfStmt *I) {
  unsigned JumpToElse =
      CompileBoolOp(static_cast<BoolOpExpr *>(I->getCondition()));
  for (auto s : I->getThen()) {
    visitStmt(s);
  }

  if (I->getElse().empty()) {
    unsigned End = Builder.getSize();
    Builder.setJumpTargetAt(JumpToElse, End);
    return;
  }
  unsigned JumpToEnd = Builder.CreateJumpForward();

  unsigned Else = Builder.getSize();
  for (auto s : I->getElse()) {
    visitStmt(s);
  }
  unsigned End = Builder.getSize();

  /// Link offset to jump.
  Builder.setJumpTargetAt(JumpToElse, Else);
  Builder.setJumpTargetAt(JumpToEnd, End);
}

void ByteCodeCompiler::visitReturn(ReturnStmt *R) {
  ChildrenVisitor::visitReturn(R);
  R->hasValue() ? Builder.CreateReturnValue() : Builder.CreateReturnNone();
}

unsigned ByteCodeCompiler::CompileBoolOp(BoolOpExpr *B) {
  if (B->hasCompareOp()) {
    auto BO = static_cast<BinOpExpr *>(B->getValue());
    visitExpr(BO->getLeft());
    visitExpr(BO->getRight());
    return Builder.CreateCondJump(BO->getOp());
  }
  ChildrenVisitor::visitBoolOp(B);
  return Builder.CreateJumpIfFalse();
}

void ByteCodeCompiler::visitCall(CallExpr *C) {
  /// Visit all the args first.
  ChildrenVisitor::visitCall(C);
  Builder.CreateCallFunction(C->getCallee(), C->getArgs().size());
}

void ByteCodeCompiler::visitStr(simplecc::StrExpr *S) {
  Builder.CreateLoadString(TheModule->getStringLiteralID(S->getStr()));
}

void ByteCodeCompiler::visitSubscript(SubscriptExpr *SB) {
  const auto &Entry = TheLocalTable[SB->getArrayName()];
  // load array
  Builder.CreateLoad(Entry.getScope(), SB->getArrayName());
  // calculate index
  visitExpr(SB->getIndex());
  // do subscript
  Builder.CreateSubscr(SB->getContext());
}

void ByteCodeCompiler::visitName(NameExpr *N) {
  const auto &Entry = TheLocalTable[N->getName()];
  if (Entry.IsConstant()) {
    Builder.CreateLoadConst(Entry.AsConstant().getValue());
    return;
  }
  N->getContext() == ExprContextKind::Load
  ? Builder.CreateLoad(Entry.getScope(), N->getName())
  : Builder.CreateStore(Entry.getScope(), N->getName());
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
    case DeclAST::FuncDefKind:visitFuncDef(static_cast<FuncDef *>(D));
      break;
    case DeclAST::VarDeclKind:
      // Collect global objects.
      TheModule->getGlobalVariables().push_back(
          TheTable->getGlobalEntry(D->getName()));
      break;
    default:break; // Ignore ConstDecl.
    }
  }
}

void ByteCodeCompiler::visitExprStmt(ExprStmt *ES) {
  ChildrenVisitor::visitExprStmt(ES);
  // discard return value
  Builder.CreatePopTop();
}

void ByteCodeCompiler::visitUnaryOp(UnaryOpExpr *U) {
  ChildrenVisitor::visitUnaryOp(U);
  Builder.CreateUnary(U->getOp());
}

void ByteCodeCompiler::visitBinOp(BinOpExpr *B) {
  ChildrenVisitor::visitBinOp(B);
  Builder.CreateBinary(B->getOp());
}

void ByteCodeCompiler::visitNum(NumExpr *N) {
  Builder.CreateLoadConst(N->getNum());
}

void ByteCodeCompiler::visitChar(CharExpr *C) {
  Builder.CreateLoadConst(C->getChar());
}

/// Visit value first and then target. The order of
/// ChildrenVisitor::visitAssign is unfortunately wrong.
void ByteCodeCompiler::visitAssign(AssignStmt *A) {
  visitExpr(A->getValue());
  visitExpr(A->getTarget());
}

void ByteCodeCompiler::Compile(ProgramAST *P, const SymbolTable &S, ByteCodeModule &M) {
  EM.clear();
  M.clear();
  setTable(&S);
  setModule(&M);
  visitProgram(P);
}