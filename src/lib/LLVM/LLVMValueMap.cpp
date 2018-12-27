#include "simplecc/LLVM/LLVMValueMap.h"

using namespace simplecc;

Constant *LLVMValueMap::getGlobalInitializer(VarDecl *VD) {
  if (VD->getIsArray()) {
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

Constant *LLVMValueMap::getConstantFromExpr(Expr *E) const {
  switch (E->getKind()) {
  case Expr::NumExprKind:
    return getInt(static_cast<NumExpr *>(E)->getN());
  case Expr::CharExprKind:
    return getChar(static_cast<CharExpr *>(E)->getC());
  default:
    llvm_unreachable("Expr must be NumExpr or CharExpr");
  }
}

Constant *LLVMValueMap::getInt(int N) const {
  return ConstantInt::get(getType(BasicTypeKind::Int), N, false);
}

Constant *LLVMValueMap::getChar(int C) const {
  return ConstantInt::get(getType(BasicTypeKind::Character), C, false);
}