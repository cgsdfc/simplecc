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
class ProgramAST;

class ByteCodeCompiler : ChildrenVisitor<ByteCodeCompiler> {
  void visitProgram(ProgramAST *P);
  void visitFuncDef(FuncDef *FD);
  /// Explicitly do nothing.
  void visitConstDecl(ConstDecl *) {}
  void visitArgDecl(ArgDecl *A);
  void visitVarDecl(VarDecl *VD);

  void visitStmt(StmtAST *S);
  void visitRead(ReadStmt *RD);
  void visitWrite(WriteStmt *WR);
  void visitAssign(AssignStmt *A);
  void visitFor(ForStmt *node);
  void visitWhile(WhileStmt *W);
  void visitIf(IfStmt *I);
  void visitReturn(ReturnStmt *R);
  void visitExprStmt(ExprStmt *ES);

  unsigned CompileBoolOp(BoolOpExpr *B);
  void visitBoolOp(BoolOpExpr *) {
    assert(false && "BoolOpExpr should be handled by CompileBoolOp()");
  }

  void visitUnaryOp(UnaryOpExpr *U);
  void visitBinOp(BinOpExpr *B);
  void visitCall(CallExpr *C);
  void visitNum(NumExpr *N);
  void visitChar(CharExpr *C);
  void visitStr(StrExpr *S);
  void visitSubscript(SubscriptExpr *SB);
  void visitName(NameExpr *N);

  void setModule(ByteCodeModule *M) { TheModule = M; }
  void setTable(const SymbolTable *S) { TheTable = S; }
  void setLocalTable(LocalSymbolTable V) { TheLocalTable = V; }

public:
  ByteCodeCompiler() = default;
  void Compile(ProgramAST *P, const SymbolTable &S, ByteCodeModule &M);

private:
  friend ChildrenVisitor;
  friend VisitorBase;

  ByteCodeBuilder Builder;
  LocalSymbolTable TheLocalTable;
  const SymbolTable *TheTable = nullptr;
  ByteCodeModule *TheModule = nullptr;
  ErrorManager EM;
};
} // namespace simplecc

#endif // SIMPLECC_CODEGEN_BYTECODECOMPILER_H