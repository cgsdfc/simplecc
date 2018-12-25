#ifndef SIMPLECOMPILER_LLVMTYPEMAP_H
#define SIMPLECOMPILER_LLVMTYPEMAP_H
#include "simplecc/Analysis/Types.h"
#include "simplecc/Parse/AST.h"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>

namespace simplecc {
using llvm::FunctionType;
using llvm::LLVMContext;
using llvm::Type;

/// This class translates simplecc's type system to LLVM's type system.
/// Provides helpers to convert objects of different types to their LLVM
/// counterparts.
class LLVMTypeMap {
  LLVMContext &TheContext;

protected:
  LLVMContext &getContext() const { return TheContext; }

public:
  explicit LLVMTypeMap(LLVMContext &Context) : TheContext(Context) {}

  /// Convert a basic type.
  Type *getType(BasicTypeKind Type) const;

  /// Convert different basic types.
  Type *getCharType() const { return getType(BasicTypeKind::Character); }
  Type *getIntType() const { return getType(BasicTypeKind::Int); }
  Type *getBoolType() const { return Type::getInt1Ty(TheContext); }

  /// Convert from Types.
  /// Convert an ArrayType.
  Type *getType(const ArrayType &A) const;

  /// Convert a VarType.
  Type *getType(const VarType &V) const { return getType(V.getType()); }
  /// Convert a ConstType.
  Type *getType(const ConstType &C) const { return getType(C.getType()); }
  /// Convert a FuncType.
  FunctionType *getType(const FuncType &F) const;

  /// Convert from Ast.
  /// Convert a FuncDef.
  FunctionType *getTypeFromFuncDef(FuncDef *FD) const {
    return getType(FuncType(FD));
  }
  /// Convert a VarDecl.
  Type *getTypeFromVarDecl(VarDecl *VD) const;
  /// Convert a ConstDecl.
  Type *getTypeFromConstDecl(ConstDecl *CD) const {
    return getType(CD->getType());
  }
};
} // namespace simplecc

#endif // SIMPLECOMPILER_LLVMTYPEMAP_H
