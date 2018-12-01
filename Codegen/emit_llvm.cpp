#include "emit_llvm.h"
#include "Visitor.h"
#include "SymbolTable.h"
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
    std::vector<llvm::Type*> ArgTypes(node->args.size());
    for (int i = 0, e = node->args.size(); i != e; ++i) {
      ArgTypes[i] = LLVMTypeFromBasicKindType(node->args[i]->type);
    }
    return llvm::FunctionType::get(RT, ArgTypes, false);
  }

  llvm::Value *LLVMConstantIntFromNum(Num *node) {
    return llvm::ConstantInt::get(Context, llvm::APInt(32, node->n));
  }

  llvm::Value *LLVMConstantIntFromChar(Char *node) {
    return llvm::ConstantInt::get(Context, llvm::APInt(8, node->c));
  }

public:
  void visitStmt(Stmt *s) { return VisitorBase::visitStmt<void>(s); }

  void visitDecl(Decl *node) { VisitorBase::visitDecl<void>(node); }

  void visitArg(Arg *node) {}

  llvm::Value *visitExpr(Expr *node) {
    return VisitorBase::visitExpr<llvm::Value*>(node);
  }

  llvm::Value *visitNum(Num *node) {
    return LLVMConstantIntFromNum(node);
  }

  llvm::Value *visitChar(Char *node) {
    return LLVMConstantIntFromChar(node);
  }

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

  llvm::Value *visitCall(Call *node) {
    auto &&Callee = Module.getFunction(node->func);
    assert(Callee);

    std::vector<llvm::Value*> ArgsV;
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
    /// Build the FunctionType
    auto &&FT = LLVMFunctionTypeFromFuncDef(node);
    auto &&F = llvm::Function::Create(
        FT, llvm::Function::ExternalLinkage, node->name, Module);

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
  LLVMIRCompiler(const SymbolTable &ST):
    Context(), Module("simplecompiler", Context), Builder(Context),
    ST(ST), Err() {}
};

}
