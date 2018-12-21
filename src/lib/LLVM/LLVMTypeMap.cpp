#include "simplecc/LLVM/LLVMTypeMap.h"

using namespace simplecc;

llvm::Type *LLVMTypeMap::getType(BasicTypeKind Type) const {
  switch (Type) {
  case BasicTypeKind::Character:return Type::getInt8Ty(TheContext);
  case BasicTypeKind::Int:return Type::getInt32Ty(TheContext);
  case BasicTypeKind::Void:return Type::getVoidTy(TheContext);
  }
}

llvm::Type *LLVMTypeMap::getTypeFromVarDecl(VarDecl *VD) const {
  if (VD->getIsArray())
    return getType(ArrayType(VD));
  return getType(VarType(VD->getType()));
}

llvm::Type *LLVMTypeMap::getType(const ArrayType &A) const {
  return llvm::ArrayType::get(getType(A.getElementType()), A.getSize());
}

llvm::FunctionType *LLVMTypeMap::getType(const FuncType &F) const {
  Type *ReturnType = getType(F.getReturnType());
  std::vector<Type *> ArgTypes(F.getArgCount());
  for (int i = 0; i < F.getArgCount(); i++) {
    ArgTypes[i] = getType(F.getArgTypeAt(i));
  }
  return FunctionType::get(ReturnType, ArgTypes, false);
}