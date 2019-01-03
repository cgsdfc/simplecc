
#include <simplecc/IR/IRCompiler.h>

#include "simplecc/IR/IRCompiler.h"
#include "simplecc/IR/Function.h"
#include "simplecc/IR/Argument.h"
#include "simplecc/IR/Module.h"

using namespace simplecc;

void IRCompiler::visitFuncDef(FuncDef *FD) {
  LocalValueMap.clear();
  auto TheFunction = Function::Create(
      /* ReturnType */ get(FD->getReturnType()),
      /* NumArgs */ FD->getNumArgs(),
      /* Name */ FD->getName(),
      /* Module */ TheModule);
  GlobalValueMap.emplace(FD->getName(), TheFunction);
  BasicBlock *EntryBlock = BasicBlock::Create(TheFunction);
  Builder.SetInsertPoint(EntryBlock);
  for (Argument *Val : TheFunction->args()) {
    visitArgDecl(FD->getArgAt(Val->getArgNo()), Val);
  }
  for (DeclAST *Decl : FD->getDecls()) {
    if (!IsInstance<VarDecl>(Decl))
      continue;
    // ConstDecl should be folded.
    visitVarDecl(static_cast<VarDecl *>(Decl), /* isLocal */ true);
  }
  for (StmtAST *Stmt : FD->getStmts()) {
    visitStmt(Stmt);
  }
  EnsureProperReturn(TheFunction);
}

Type *IRCompiler::get(BasicTypeKind Ty) const {
  switch (Ty) {
  case BasicTypeKind::Int:
  case BasicTypeKind::Character: return Builder.getIntType();
  case BasicTypeKind::Void: return Builder.getVoidType();
  }
}

void IRCompiler::visitArgDecl(ArgDecl *AD, Argument *Val) {
  auto Ptr = Builder.CreateAlloca(1);
  Builder.CreateStore(Val, Ptr);
  LocalValueMap.emplace(AD->getName(), Ptr);
}

void IRCompiler::visitVarDecl(VarDecl *VD, bool isLocal) {
  if (isLocal) {
    auto Ptr = Builder.CreateAlloca(getSize(VD));
    LocalValueMap.emplace(VD->getName(), Ptr);
    return;
  }
}

unsigned int IRCompiler::getSize(VarDecl *VD) {
  return VD->isArray() ? VD->getSize() : 1;
}

Value *IRCompiler::get(SymbolEntry Entry) {
  auto Val = Entry.IsGlobal() ?
             GlobalValueMap[Entry.getName()] : LocalValueMap[Entry.getName()];
  assert(Val && "Absent Value");
  return Val;
}

void IRCompiler::EnsureProperReturn(Function *F) {
  for (BasicBlock *BB : *F) {
    Instruction *Terminator = BB->getTerminator();
    if (Terminator != nullptr)
      continue;
    Builder.SetInsertPoint(BB);
    switch (F->getReturnType()->getTypeID()) {
    case Type::IntTypeKind:Builder.CreateRet(Builder.getInt32(0));
      break;
    case Type::VoidTypeKind:Builder.CreateRetVoid();
      break;
    default:assert(false && "Impossible Return Type");
    }
  }
}
void IRCompiler::visitProgram(Program *P) {
  ChildrenVisitor::visitProgram(P);
}
void IRCompiler::visitConstDecl(ConstDecl *CD) {
  ChildrenVisitor::visitConstDecl(CD);
}
void IRCompiler::visitWrite(WriteStmt *WR) {
  ChildrenVisitor::visitWrite(WR);
}
void IRCompiler::visitRead(ReadStmt *RD) {
  ChildrenVisitor::visitRead(RD);
}
void IRCompiler::visitAssign(AssignStmt *A) {
  ChildrenVisitor::visitAssign(A);
}
void IRCompiler::visitReturn(ReturnStmt *Ret) {
  ChildrenVisitor::visitReturn(Ret);
}
void IRCompiler::visitFor(ForStmt *F) {
  ChildrenVisitor::visitFor(F);
}
void IRCompiler::visitWhile(WhileStmt *W) {
  ChildrenVisitor::visitWhile(W);
}
void IRCompiler::visitIf(IfStmt *I) {
  ChildrenVisitor::visitIf(I);
}
bool IRCompiler::visitStmtList(const StmtAST::StmtListType &StmtList) {
  return false;
}
Value *IRCompiler::visitBinOp(BinOpExpr *B) {
}
Value *IRCompiler::visitUnaryOp(UnaryOpExpr *U) {
}
Value *IRCompiler::visitSubscript(SubscriptExpr *S) {
}
Value *IRCompiler::visitCall(CallExpr *C) {
}
Value *IRCompiler::visitBoolOp(BoolOpExpr *B) {
}
Value *IRCompiler::visitParenExpr(ParenExpr *P) {
}
Value *IRCompiler::visitNum(NumExpr *N) {
}
Value *IRCompiler::visitChar(CharExpr *C) {
}
Value *IRCompiler::visitName(NameExpr *N) {
}
Value *IRCompiler::visitStr(StrExpr *S) {
}
bool IRCompiler::Compile(Program *P, const SymbolTable &S, Module &M) {
  return false;
}
