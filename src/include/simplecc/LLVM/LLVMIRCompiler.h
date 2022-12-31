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

#ifndef SIMPLECC_LLVM_LLVMIRCOMPILER_H
#define SIMPLECC_LLVM_LLVMIRCOMPILER_H
#include "simplecc/AST/VisitorBase.h"
#include "simplecc/Analysis/SymbolTable.h"
#include "simplecc/LLVM/LLVMValueMap.h"
#include "simplecc/Support/ErrorManager.h"
#include <llvm/IR/IRBuilder.h>
#include <unordered_map>

namespace llvm {
class Value;
class Function;
} // namespace llvm

namespace simplecc {
using llvm::Function;
using llvm::IRBuilder;
using llvm::LLVMContext;
using llvm::Value;

/// This class emits LLVM IR from an AST.
/// The principle it enforces is:
/// 1. Don't change the semantic of the user's program.
/// 2. Be well-formed in terms of LLVM IR.
/// 3. Avoid useless stuffs.
class LLVMIRCompiler : VisitorBase<LLVMIRCompiler> {
  /// @brief Generates code for the whole program.
  void visitProgram(ProgramAST *P);

  /// @brief Noop.
  void visitArgDecl(ArgDecl *) {}
  void visitVarDecl(VarDecl *VD);
  void visitConstDecl(ConstDecl *CD);
  void visitFuncDef(FuncDef *FD);

  void visitWrite(WriteStmt *W);
  void visitRead(ReadStmt *RD);
  void visitAssign(AssignStmt *A);
  void visitReturn(ReturnStmt *Ret);
  void visitFor(ForStmt *F);
  void visitWhile(WhileStmt *W);
  void visitIf(IfStmt *I);

  void visitExprStmt(ExprStmt *ES) {
    visitExpr(ES->getValue());
  }

  Value *visitExpr(ExprAST *E) {
    return VisitorBase::visitExpr<Value *>(E);
  }

  Value *visitBinOp(BinOpExpr *B);
  Value *visitUnaryOp(UnaryOpExpr *U);
  Value *visitSubscript(SubscriptExpr *SB);
  Value *visitCall(CallExpr *C);
  Value *visitBoolOp(BoolOpExpr *B);

  Value *visitParenExpr(ParenExpr *PE) {
    return visitExprPromoteToInt(PE->getValue());
  }

  Value *visitNum(NumExpr *N) {
    return VM.getInt(N->getNum());
  }

  Value *visitChar(CharExpr *C) {
    return VM.getChar(C->getChar());
  }

  Value *visitName(NameExpr *N);
  Value *visitStr(StrExpr *S);

  /// @brief PromoteToInt accepts an int or char and cast it to an int.
  Value *PromoteToInt(Value *Val);

  /// @brief visitExprPromoteToInt generates code for an expression and ensures the result is an int.
  Value *visitExprPromoteToInt(ExprAST *E) {
    return PromoteToInt(visitExpr(E));
  }

  /// @brief Generates code for a list of statements.
  /// @return true if \param StmtList does not contain a ReturnStmt.
  bool visitStmtList(const StmtAST::StmtListType &StmtList);

  /// @brief Inserts declarations for builtin functions such as ``printf()`` and ``scanf()``
  /// into the Module.
  void DeclareBuiltinFunctions();

  /// @brief Declares an IO builtin by inserting it into the Module.
  Function *DeclareIOBuiltins(const char *Name);

  /// @brief Converts a string literal to a llvm string constant.
  Value *getString(llvm::StringRef Str);

  /// @brief Return a format specifier for printf() to use.
  const char *getPrintfFmtSpec(const WriteStmt *W) const;

  /// @brief Return a format specifier for scanf() to use.
  const char *getScanfFmtSpec(const NameExpr *N) const;

public:
  /// Don't put instance on the stack. It is about 1K.
  LLVMIRCompiler(ProgramAST *P, const SymbolTable &S);

  LLVMIRCompiler(const LLVMIRCompiler &) = delete;
  LLVMIRCompiler(LLVMIRCompiler &&) = delete;

  /// Force a out-of-line destructor.
  ~LLVMIRCompiler();

  /// Compile the program, return true if errors happened.
  bool Compile();

  /// Access the compiled Module.
  llvm::Module &getModule() { return TheModule; }
  const llvm::Module &getModule() const { return TheModule; }

  /// Access the Context.
  llvm::LLVMContext &getContext() { return TheContext; }
  const llvm::LLVMContext &getContext() const { return TheContext; }

  /// Access the Program being compiled.
  const ProgramAST *getProgram() const { return TheProgram; }
  ProgramAST *getProgram() { return TheProgram; }

private:
  /// Data members:
  friend VisitorBase;

  /// Used for getting types of an Expr.
  const SymbolTable &TheTable;
  /// AST tree node to be visited.
  ProgramAST *TheProgram;
  LocalSymbolTable TheLocal;

  /// LLVM core data structures.
  LLVMContext TheContext;
  /// The Module to be populated.
  llvm::Module TheModule;
  /// High Level Instruction Builder.
  IRBuilder<> Builder;

  /// Type & Value conversion helper.
  LLVMValueMap VM;

  /// Keep track of local name binding.
  /// Local Constant => ConstantInt.
  /// Local Array/Variable => Alloca(Type, ArraySize=nullptr).
  /// Global Stuffs => As it in GlobalValues.
  std::unordered_map<std::string, Value *> LocalValues;

  /// Keep track of global name binding.
  /// Global Constant => GlobalVariable(IsConstant=true, ExternalLinkage).
  /// Global Array => GlobalVariable(Initializer=ConstantAggregateZero,
  /// ExternalLinkage).
  /// Global Variable => GlobalVariable(Initializer=Zero, ExternalLinkage).
  /// Global Function => Function(ExternalLinkage).
  /// printf/scanf => External Function Declaration.
  std::unordered_map<std::string, Value *> GlobalValues;

  /// Error handling. There should not be any user's errors in the stage.
  /// But developer can make mistakes and this EM will tell.
  ErrorManager EM;
};

} // namespace simplecc
#endif // SIMPLECC_LLVM_LLVMIRCOMPILER_H