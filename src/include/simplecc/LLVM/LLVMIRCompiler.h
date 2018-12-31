#ifndef SIMPLECC_LLVM_LLVMIRCOMPILER_H
#define SIMPLECC_LLVM_LLVMIRCOMPILER_H
#include "simplecc/Analysis/Visitor.h"
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
///
/// The principle it enforces is:
/// 1. Don't change the semantic of the user's program.
/// 2. Be well-formed in terms of LLVM IR.
/// 3. Avoid useless stuffs.
class LLVMIRCompiler : VisitorBase<LLVMIRCompiler> {

  bool visitStmtList(const std::vector<StmtAST *> &StatementList);
  Value *visitUnaryOp(UnaryOpExpr *U);

  /// Declare builtin functions (external really, but let's call it builtin).
  void DeclareBuiltinFunctions();
  Function *DeclareIOBuiltins(const char *Name);

  Value *getString(llvm::StringRef Str);
  Value *visitExpr(ExprAST *E) { return VisitorBase::visitExpr<Value *>(E); }
  void visitArgDecl(ArgDecl *) {}
  void visitProgram(Program *P);
  void visitVarDecl(VarDecl *VD);
  void visitConstDecl(ConstDecl *CD);

  /// Generate body for a Function.
  void visitFuncDef(FuncDef *FD);

  /// Emit a printf("%d\n", Var) for printf(<int-expr>);
  ///      a printf("%c\n", Var) for printf(<char-expr>);
  ///      a printf("%s\n", Str) for printf(<string>);
  ///      a printf("%s%d\n", Str, Var) for printf(<string>, <int-expr>);
  ///      a printf("%s%c\n", Str, Var) for printf(<string>, <char-expr>);
  void visitWrite(WriteStmt *WR);
  void visitRead(ReadStmt *RD);

  /// The logic varies depending on whether it is a load/store
  Value *visitSubscript(SubscriptExpr *SB);
  void visitAssign(AssignStmt *A);
  void visitReturn(ReturnStmt *Ret);
  Value *visitCall(CallExpr *C);

  /// For is the most complicated beast, and with the requirement to
  /// execute the body **before** the evaluation of condition at first
  /// make it no like ordinary for.
  void visitFor(ForStmt *F);
  void visitWhile(WhileStmt *W);
  void visitIf(IfStmt *I);

  /// BinOp requires both operands to be int's.
  Value *visitBinOp(BinOpExpr *B);

  /// This method accept an int or char and cast it to an int.
  Value *PromoteToInt(Value *Val);

  /// This helper evaluates an Expr, optional emits a char-to-int cast
  /// to ensure the result is int.
  Value *visitExprPromoteToInt(ExprAST *E) { return PromoteToInt(visitExpr(E)); }

  /// The tricky part of BoolOp:
  /// BoolOp has two forms, indicated by getHasCmpop() and should be handled
  /// separately: The mission of visitBoolOp() is to evaluate the condition
  /// expression and produce a bool value that indicates whether the condition
  /// is true. We have these in grammar:
  /// Form-1: <Expr> <RichCompareOp> <Expr> => bool -- already a bool.
  /// Form-2: <Expr> => int -- not a bool yet, compare it to int(0).
  Value *visitBoolOp(BoolOpExpr *B);

  void visitExprStmt(ExprStmt *ES) { visitExpr(ES->getValue()); }

  /// ParenExpr automatically promotes a char to int.
  Value *visitParenExpr(ParenExpr *PE) {
    return visitExprPromoteToInt(PE->getValue());
  }

  Value *visitName(NameExpr *Nn);
  Value *visitStr(StrExpr *S);

  /// Simple atom nodes.
  Value *visitNum(NumExpr *N) { return VM.getInt(N->getN()); }
  Value *visitChar(CharExpr *C) { return VM.getChar(C->getC()); }

private:
  /// Data members:
  friend class VisitorBase<LLVMIRCompiler>;

  /// Used for getting types of an Expr.
  const SymbolTable &TheTable;
  /// AST tree node to be visited.
  Program *TheProgram;

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
  /// printf/scanf => External Function Declaration, Function(ExternalLinkage,
  /// BasicBlocks=None).
  std::unordered_map<std::string, Value *> GlobalValues;

  /// Error handling. There should not be any user's errors in the stage.
  /// But we developer can make mistakes and this EM will tell.
  ErrorManager EM;

public:
  /// Don't put instance on the stack. It is about 1K.
  LLVMIRCompiler(Program *P, const SymbolTable &S);

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
  const Program *getProgram() const { return TheProgram; }
  Program *getProgram() { return TheProgram; }
};

} // namespace simplecc

#endif // SIMPLECC_LLVM_LLVMIRCOMPILER_H
