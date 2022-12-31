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

#ifndef SIMPLECC_CODEGEN_BYTECODECOMPILER_H
#define SIMPLECC_CODEGEN_BYTECODECOMPILER_H
#include "simplecc/Analysis/SymbolTable.h"
#include "simplecc/AST/ChildrenVisitor.h"
#include "simplecc/CodeGen/ByteCodeBuilder.h"
#include "simplecc/Support/ErrorManager.h"
#include <cassert>
#include <utility>

namespace simplecc {
class ByteCodeModule;

/// @brief ByteCodeCompiler compiles an AST into a ByteCodeModule.
class ByteCodeCompiler : ChildrenVisitor<ByteCodeCompiler> {
  void visitProgram(ProgramAST *P);

  /// Compile DeclAST.
  void visitFuncDef(FuncDef *FD);
  void visitConstDecl(ConstDecl *) {}
  void visitArgDecl(ArgDecl *A);
  void visitVarDecl(VarDecl *VD);

  /// Compile StmtAST.
  void visitStmt(StmtAST *S);
  void visitRead(ReadStmt *RD);
  void visitWrite(WriteStmt *WR);
  void visitAssign(AssignStmt *A);
  void visitFor(ForStmt *F);
  void visitWhile(WhileStmt *W);
  void visitIf(IfStmt *I);
  void visitReturn(ReturnStmt *R);
  void visitExprStmt(ExprStmt *ES);

  /// Compile ExprAST.
  void visitUnaryOp(UnaryOpExpr *U);
  void visitBinOp(BinOpExpr *B);
  void visitCall(CallExpr *C);
  void visitNum(NumExpr *N);
  void visitChar(CharExpr *C);
  void visitStr(StrExpr *S);
  void visitSubscript(SubscriptExpr *SB);
  void visitName(NameExpr *N);

  /// Compile a BoolOpExpr. Replacement of visitBoolOp().
  unsigned CompileBoolOp(BoolOpExpr *B);
  void visitBoolOp(BoolOpExpr *) {
    assert(false && "BoolOpExpr should be handled by CompileBoolOp()");
  }

  /// Setters.
  void setModule(ByteCodeModule *M) { TheModule = M; }
  void setTable(const SymbolTable *S) { TheTable = S; }
  void setLocalTable(LocalSymbolTable V) { TheLocalTable = V; }

public:
  ByteCodeCompiler() = default;
  /// Compile a program into a ByteCodeModule.
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