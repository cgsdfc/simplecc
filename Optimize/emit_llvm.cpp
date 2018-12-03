#include "emit_llvm.h"
#include "SymbolTable.h"
#include "Visitor.h"
#include "error.h"

#include <llvm/ADT/APSInt.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

using namespace simplecompiler;

namespace {

class LLVMIRCompiler : public VisitorBase<LLVMIRCompiler> {
  /// llvm specific members
  llvm::LLVMContext Context;
  llvm::Module Module;
  llvm::IRBuilder<> Builder;

  const SymbolTable &ST;
  ErrorManager Err;
  /* SymbolTableView local; */

  /* void SetLocal(FuncDef *FD) { */
  /*   local = ST.GetLocal(FD); */
  /* } */

  /// Convert a BasicTypeKind to corresponding llvm Type
  llvm::Type *LLVMTypeFromBasicKindType(BasicTypeKind T) {
    switch (T) {
    case BasicTypeKind::Character:
      return llvm::Type::getInt8Ty(Context);
    case BasicTypeKind::Int:
      return llvm::Type::getInt32Ty(Context);
    case BasicTypeKind::Void:
      return llvm::Type::getVoidTy(Context);
    }
  }

  /// Convert a FuncDef node to corresponding llvm FunctionType
  llvm::FunctionType *LLVMFunctionTypeFromFuncDef(FuncDef *node) {
    auto &&RT = LLVMTypeFromBasicKindType(node->return_type);
    std::vector<llvm::Type *> ArgTypes(node->args.size());
    for (int i = 0, e = node->args.size(); i != e; ++i) {
      ArgTypes[i] = LLVMTypeFromBasicKindType(node->args[i]->type);
    }
    return llvm::FunctionType::get(RT, ArgTypes, false);
  }

  llvm::Value *I32FromInt(int I) {
    return llvm::ConstantInt::get(Context, llvm::APInt(32, I));
  }

  llvm::Value *I8FromInt(int I) {
    return llvm::ConstantInt::get(Context, llvm::APInt(8, I));
  }

public:
  void visitStmt(Stmt *s) { return VisitorBase::visitStmt<void>(s); }

  void visitDecl(Decl *node) { VisitorBase::visitDecl<void>(node); }

  void visitArg(Arg *node) {}

  llvm::Value *visitExpr(Expr *node) {
    return VisitorBase::visitExpr<llvm::Value *>(node);
  }

  llvm::Value *visitNum(Num *node) { return I32FromInt(node->n); }

  llvm::Value *visitChar(Char *node) { return I8FromInt(node->c); }

  llvm::Value *visitBinOp(BinOp *node) {
    auto L = visitExpr(node->left);
    auto R = visitExpr(node->right);
    assert(L && R);
    switch (node->op) {
    case OperatorKind::Add:
      return Builder.CreateAdd(L, R, "addtmp");
    case OperatorKind::Sub:
      return Builder.CreateSub(L, R, "subtmp");
    case OperatorKind::Mult:
      return Builder.CreateMul(L, R, "multmp");
    case OperatorKind::Div:
      return Builder.CreateUDiv(L, R, "udivtmp");
    case OperatorKind::Eq:
      return Builder.CreateICmpEQ(L, R, "eqtmp");
    case OperatorKind::NotEq:
      return Builder.CreateICmpNE(L, R, "netmp");
    case OperatorKind::Lt:
      return Builder.CreateICmpSLT(L, R, "slttmp");
    case OperatorKind::LtE:
      return Builder.CreateICmpSLE(L, R, "sletmp");
    case OperatorKind::Gt:
      return Builder.CreateICmpSGT(L, R, "sgttmp");
    case OperatorKind::GtE:
      return Builder.CreateICmpSGE(L, R, "sgetmp");
    }
  }

  llvm::Value *visitUnaryOp(UnaryOp *node) {
    auto &&Operand = visitExpr(node->operand);
    switch (node->op) {
    case UnaryopKind::USub:
      return Builder.CreateNeg(Operand, "negtmp");
    case UnaryopKind::UAdd:
      return Operand;
    }
  }

  llvm::Value *visitParenExpr(ParenExpr *node) {
    return visitExpr(node->value);
  }

  llvm::Value *visitBoolOp(BoolOp *node) { return visitExpr(node->value); }

  void visitIf(If *node) {
    auto &&CondV = visitExpr(node->test);
    CondV = Builder.CreateICmpNE(
        CondV, llvm::ConstantInt::get(Context, llvm::APInt(1, 0)), "ifcond");
    auto &&TheFunction = Builder.GetInsertBlock()->getParent();

    auto &&ThenBB = llvm::BasicBlock::Create(Context, "then", TheFunction);
    auto &&ElseBB = llvm::BasicBlock::Create(Context, "else");
    auto &&MergeBB = llvm::BasicBlock::Create(Context, "ifcont");

    Builder.CreateCondBr(CondV, ThenBB, ElseBB);
    Builder.SetInsertPoint(ThenBB);
    for (auto &&S : node->body) {
      visitStmt(S);
    }
    Builder.CreateBr(MergeBB);

    TheFunction->getBasicBlockList().push_back(ElseBB);
    Builder.SetInsertPoint(ElseBB);
    for (auto &&S : node->orelse) {
      visitStmt(S);
    }
    Builder.CreateBr(MergeBB);

    TheFunction->getBasicBlockList().push_back(MergeBB);
  }

  void visitWhile(While *node) {
    auto &&CondV = visitExpr(node->condition);
    auto &&BodyBB = llvm::BasicBlock::Create(Context, "body");
    auto &&EndBB = llvm::BasicBlock::Create(Context, "endWhile");
    Builder.CreateCondBr(CondV, BodyBB, EndBB);
    Builder.SetInsertPoint(BodyBB);
    for (auto &&S : node->body) {
      visitStmt(S);
    }
    Builder.SetInsertPoint(EndBB);
  }

  /* llvm::Value *visitName(Name *node) { */
  /*   auto &&Entry = local[node->id]; */
  /*   if (Entry.IsConstant()) { */
  /*     auto &&CT = Entry.AsConstant(); */
  /*     return CT.GetType() == BasicTypeKind::Int ? I32FromInt(CT.GetValue())
   */
  /*                                               : I8FromInt(CT.GetValue());
   */
  /*   } */

  /* } */

  llvm::Value *visitCall(Call *node) {
    auto &&Callee = Module.getFunction(node->func);
    assert(Callee);

    std::vector<llvm::Value *> ArgsV;
    ArgsV.reserve(node->args.size());
    for (auto &&arg : node->args) {
      auto &&V = visitExpr(arg);
      assert(V);
      ArgsV.push_back(V);
    }

    return Builder.CreateCall(Callee, ArgsV, "calltmp");
  }

  llvm::Value *visitReturn(Return *node) {
    if (node->value) {
      auto &&Val = visitExpr(node->value);
      assert(Val);
      return Builder.CreateRet(Val);
    } else {
      return Builder.CreateRetVoid();
    }
  }

  llvm::Function *visitFuncDef(FuncDef *node) {
    /* SetLocal(node); */
    /// Build the FunctionType
    auto &&FT = LLVMFunctionTypeFromFuncDef(node);
    auto &&F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                      node->name, Module);

    /// Create entry point
    auto &&BB = llvm::BasicBlock::Create(Context, "entry", F);
    Builder.SetInsertPoint(BB);

    for (auto &&Arg : node->args) {
      visitArg(Arg);
    }
    for (auto &&Decl : node->decls) {
      visitDecl(Decl);
    }
    for (auto &&Stmt : node->stmts) {
      visitStmt(Stmt);
    }

    /// Verify the function body
    String ErrorMsg;
    llvm::raw_string_ostream OS(ErrorMsg);
    if (llvm::verifyFunction(*F, &OS)) {
      Err.Error(ErrorMsg);
      F->eraseFromParent();
      return nullptr;
    }
    return F;
  }

public:
  LLVMIRCompiler(const SymbolTable &ST)
      : Context(), Module("simplecompiler", Context), Builder(Context), ST(ST),
        Err() {}
};

} // namespace
