#include "simplecc/LLVM/LLVMValueMap.h"

using namespace simplecc;

Constant *LLVMValueMap::getGlobalInitializer(VarDecl *VD) {
  if (VD->isArray()) {
    return llvm::ConstantAggregateZero::get(getTypeFromVarDecl(VD));
  }
  switch (VD->getType()) {
  case BasicTypeKind::Int:
    return getInt(0);
  case BasicTypeKind::Character:
    return getChar(0);
  default:
    llvm_unreachable("Void cannot be");
  }
}

Constant *LLVMValueMap::getConstantFromExpr(ExprAST *E) const {
  switch (E->getKind()) {
  case ExprAST::NumExprKind:
    return getInt(static_cast<NumExpr *>(E)->getNum());
  case ExprAST::CharExprKind:
    return getChar(static_cast<CharExpr *>(E)->getChar());
  default:
    llvm_unreachable("ExprAST must be NumExpr or CharExpr");
  }
}

Constant *LLVMValueMap::getInt(int N) const {
  return ConstantInt::get(getType(BasicTypeKind::Int), N, false);
}

Constant *LLVMValueMap::getChar(int C) const {
  return ConstantInt::get(getType(BasicTypeKind::Character), C, false);
}