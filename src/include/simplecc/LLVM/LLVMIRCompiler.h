#ifndef SIMPLECC_LLVM_LLVMIRCOMPILER_H
#define SIMPLECC_LLVM_LLVMIRCOMPILER_H
#include "simplecc/AST/Visitor.h"
#include "simplecc/LLVM/LLVMValueMap.h"
#include "simplecc/Support/ErrorManager.h"
#include <llvm/ADT/StringRef.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <unordered_map>

namespace llvm {
class Value;
class Function;
} // namespace llvm

namespace simplecc {
class SymbolTable;
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
  void visitProgram(ProgramAST *P);
  void visitArgDecl(ArgDecl *) {}
  void visitVarDecl(VarDecl *VD);
  void visitConstDecl(ConstDecl *CD);
  void visitFuncDef(FuncDef *FD);

  void visitWrite(WriteStmt *WR);
  void visitRead(ReadStmt *RD);
  void visitAssign(AssignStmt *A);
  void visitReturn(ReturnStmt *Ret);
  void visitFor(ForStmt *F);
  void visitWhile(WhileStmt *W);
  void visitIf(IfStmt *I);
  void visitExprStmt(ExprStmt *ES) { visitExpr(ES->getValue()); }

  Value *visitExpr(ExprAST *E) { return VisitorBase::visitExpr<Value *>(E); }
  Value *visitBinOp(BinOpExpr *B);
  Value *visitUnaryOp(UnaryOpExpr *U);
  Value *visitSubscript(SubscriptExpr *SB);
  Value *visitCall(CallExpr *C);
  Value *visitBoolOp(BoolOpExpr *B);
  Value *visitParenExpr(ParenExpr *PE) {
    return visitExprPromoteToInt(PE->getValue());
  }

  /// Simple atom nodes.
  Value *visitNum(NumExpr *N) { return VM.getInt(N->getNum()); }
  Value *visitChar(CharExpr *C) { return VM.getChar(C->getChar()); }
  Value *visitName(NameExpr *Nn);
  Value *visitStr(StrExpr *S);

  /// This method accept an int or char and cast it to an int.
  Value *PromoteToInt(Value *Val);

  /// This helper evaluates an Expr, optional emits a char-to-int cast
  /// to ensure the result is int.
  Value *visitExprPromoteToInt(ExprAST *E) {
    return PromoteToInt(visitExpr(E));
  }

  bool visitStmtList(const std::vector<StmtAST *> &StatementList);
  void DeclareBuiltinFunctions();
  Function *DeclareIOBuiltins(const char *Name);
  Value *getString(llvm::StringRef Str);

public:
  /// Don't put instance on the stack. It is about 1K.
  LLVMIRCompiler(ProgramAST *P, const SymbolTable &S);

  /// No copy no move.
  LLVMIRCompiler(const LLVMIRCompiler &) = delete;
  LLVMIRCompiler &operator=(const LLVMIRCompiler &) = delete;
  LLVMIRCompiler(LLVMIRCompiler &&) = delete;
  LLVMIRCompiler &operator=(LLVMIRCompiler &&) = delete;

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