#ifndef SIMPLECOMPILER_BYTECODECOMPILER_H
#define SIMPLECOMPILER_BYTECODECOMPILER_H
#include "simplecc/Analysis/SymbolTable.h"
#include "ByteCodeBuilder.h"
#include "simplecc/Support/ErrorManager.h"
#include "simplecc/Analysis/Visitor.h"

#include <cassert>
#include <utility>

namespace simplecc {
class ByteCodeModule;
class Program;

class ByteCodeCompiler : ChildrenVisitor<ByteCodeCompiler> {

  void visitStmt(Stmt *S);

  void visitArgDecl(ArgDecl *A);

  void visitVarDecl(VarDecl *VD);

  /// Explicitly do nothing.
  void visitConstDecl(ConstDecl *) {}

  void visitRead(Read *RD);

  void visitWrite(Write *WR);

  /// Visit value first and then target. The order of
  /// ChildrenVisitor::visitAssign is unfortunately wrong.
  void visitAssign(Assign *A) {
    visitExpr(A->getValue());
    visitExpr(A->getTarget());
  }

  unsigned CompileBoolOp(BoolOp *B);

  void visitFor(For *node);

  void visitWhile(While *W);

  void visitIf(If *I);

  void visitReturn(Return *R);

  void visitExprStmt(ExprStmt *ES) {
    ChildrenVisitor::visitExprStmt(ES);
    // discard return value
    Builder.CreatePopTop();
  }

  void visitBoolOp(BoolOp *) {
    assert(false && "BoolOp should be handled by CompileBoolOp()");
  }

  void visitUnaryOp(UnaryOp *U) {
    ChildrenVisitor::visitUnaryOp(U);
    Builder.CreateUnary(U->getOp());
  }

  void visitBinOp(BinOp *B) {
    ChildrenVisitor::visitBinOp(B);
    Builder.CreateBinary(B->getOp());
  }

  void visitCall(Call *C);
  void visitNum(Num *N) { Builder.CreateLoadConst(N->getN()); }
  void visitChar(Char *C) { Builder.CreateLoadConst(C->getC()); }
  void visitStr(Str *S);

  void visitSubscript(Subscript *SB);

  void visitName(Name *N);

  void visitFuncDef(FuncDef *FD);

  void visitProgram(Program *P);

  void setModule(ByteCodeModule *M) { TheModule = M; }
  void setTable(const SymbolTable *S) { TheTable = S; }
  void setLocalTable(SymbolTableView V) { TheLocalTable = V; }

public:
  ByteCodeCompiler() = default;

  // public interface
  void Compile(Program *P, const SymbolTable &S, ByteCodeModule &M);

private:
  friend class ChildrenVisitor<ByteCodeCompiler>;
  friend class VisitorBase<ByteCodeCompiler>;

  ByteCodeBuilder Builder;
  SymbolTableView TheLocalTable;
  const SymbolTable *TheTable = nullptr;
  ByteCodeModule *TheModule = nullptr;
  ErrorManager EM;
};
}

#endif //SIMPLECOMPILER_BYTECODECOMPILER_H
