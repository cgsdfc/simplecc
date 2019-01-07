#ifndef SIMPLECC_IR_IRCOMPILER_H
#define SIMPLECC_IR_IRCOMPILER_H
#include "simplecc/Analysis/ContextualVisitor.h"
#include "simplecc/IR/IRBuilder.h"
#include "Argument.h"
#include <unordered_map>

namespace simplecc {
class Module;
class IRCompiler : ContextualVisitor<IRCompiler> {
  void visitProgram(ProgramAST *P);

  void visitDecl(DeclAST *) {}
  void visitFuncDef(FuncDef *FD);
  void visitArgDecl(ArgDecl *AD, Argument *Val);
  void visitVarDecl(VarDecl *VD, bool isLocal);
  void visitConstDecl(ConstDecl *CD) {}

  void visitWrite(WriteStmt *WR);
  void visitRead(ReadStmt *RD);
  void visitAssign(AssignStmt *A);
  void visitReturn(ReturnStmt *R);
  void visitFor(ForStmt *F);
  void visitWhile(WhileStmt *W);
  void visitIf(IfStmt *I);
  void visitStmtList(const StmtAST::StmtListType &StmtList);

  Value *visitExpr(ExprAST *E) { return VisitorBase::visitExpr<Value *>(E); }
  Value *visitBinOp(BinOpExpr *B);
  Value *visitUnaryOp(UnaryOpExpr *U);
  Value *visitSubscript(SubscriptExpr *S);
  Value *visitCall(CallExpr *C);
  Value *visitBoolOp(BoolOpExpr *B);
  Value *visitParenExpr(ParenExpr *P);
  Value *visitNum(NumExpr *N);
  Value *visitChar(CharExpr *C);
  Value *visitName(NameExpr *N);
  Value *visitStr(StrExpr *S);

  unsigned int getSize(VarDecl *VD);
  void EnsureProperReturn(Function *F);
  Value *maybeLoad(Value *Ptr, ExprContextKind ExprContext);
  Type *get(BasicTypeKind Ty) const;
  Value *get(SymbolEntry Entry);
  Value *get(const std::string &Name) {
    return get(getSymbolEntry(Name));
  }

public:
  IRCompiler() = default;
  bool Compile(ProgramAST *P, const SymbolTable &S, Module &M);
private:
  friend VisitorBase;
  friend ChildrenVisitor;
  Module *TheModule;
  IRContext *TheContext;
  IRBuilder Builder;
  std::unordered_map<std::string, Value *> GlobalValueMap;
  std::unordered_map<std::string, Value *> LocalValueMap;

};

}
#endif //SIMPLECC_IR_IRCOMPILER_H