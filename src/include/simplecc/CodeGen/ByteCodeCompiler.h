#ifndef SIMPLECC_CODEGEN_BYTECODECOMPILER_H
#define SIMPLECC_CODEGEN_BYTECODECOMPILER_H
#include "simplecc/Analysis/SymbolTable.h"
#include "simplecc/Analysis/Visitor.h"
#include "simplecc/CodeGen/ByteCodeBuilder.h"
#include "simplecc/Support/ErrorManager.h"

#include <cassert>
#include <utility>

namespace simplecc {
class ByteCodeModule;
class Program;

class ByteCodeCompiler : ChildrenVisitor<ByteCodeCompiler> {

  void visitStmt(StmtAST *S);
  void visitArgDecl(ArgDecl *A);
  void visitVarDecl(VarDecl *VD);

  /// Explicitly do nothing.
  void visitConstDecl(ConstDecl *) {}
  void visitRead(ReadStmt *RD);
  void visitWrite(WriteStmt *WR);

  /// Visit value first and then target. The order of
  /// ChildrenVisitor::visitAssign is unfortunately wrong.
  void visitAssign(AssignStmt *A) {
    visitExpr(A->getValue());
    visitExpr(A->getTarget());
  }

  unsigned CompileBoolOp(BoolOpExpr *B);

  void visitFor(ForStmt *node);
  void visitWhile(WhileStmt *W);
  void visitIf(IfStmt *I);
  void visitReturn(ReturnStmt *R);

  void visitExprStmt(ExprStmt *ES) {
    ChildrenVisitor::visitExprStmt(ES);
    // discard return value
    Builder.CreatePopTop();
  }

  void visitBoolOp(BoolOpExpr *) {
    assert(false && "BoolOpExpr should be handled by CompileBoolOp()");
  }

  void visitUnaryOp(UnaryOpExpr *U) {
    ChildrenVisitor::visitUnaryOp(U);
    Builder.CreateUnary(U->getOp());
  }

  void visitBinOp(BinOpExpr *B) {
    ChildrenVisitor::visitBinOp(B);
    Builder.CreateBinary(B->getOp());
  }

  void visitCall(CallExpr *C);
  void visitNum(NumExpr *N) { Builder.CreateLoadConst(N->getN()); }
  void visitChar(CharExpr *C) { Builder.CreateLoadConst(C->getC()); }
  void visitStr(StrExpr *S);

  void visitSubscript(SubscriptExpr *SB);
  void visitName(NameExpr *N);
  void visitFuncDef(FuncDef *FD);
  void visitProgram(Program *P);

  void setModule(ByteCodeModule *M) { TheModule = M; }
  void setTable(const SymbolTable *S) { TheTable = S; }
  void setLocalTable(LocalSymbolTable V) { TheLocalTable = V; }

public:
  ByteCodeCompiler() = default;

  // public interface
  void Compile(Program *P, const SymbolTable &S, ByteCodeModule &M);

private:
  friend class ChildrenVisitor<ByteCodeCompiler>;
  friend class VisitorBase<ByteCodeCompiler>;

  ByteCodeBuilder Builder;
  LocalSymbolTable TheLocalTable;
  const SymbolTable *TheTable = nullptr;
  ByteCodeModule *TheModule = nullptr;
  ErrorManager EM;
};
} // namespace simplecc

#endif // SIMPLECC_CODEGEN_BYTECODECOMPILER_H
