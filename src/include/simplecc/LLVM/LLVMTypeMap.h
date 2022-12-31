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

#ifndef SIMPLECC_LLVM_LLVMTYPEMAP_H
#define SIMPLECC_LLVM_LLVMTYPEMAP_H
#include "simplecc/Analysis/Types.h"
#include "simplecc/AST/AST.h"
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>

namespace simplecc {
using llvm::FunctionType;
using llvm::LLVMContext;
using llvm::Type;

/// @brief LLVMTypeMap translates simplecc's type system to LLVM's type system.
/// Provides helpers to convert objects of different types to their LLVM counterparts.
class LLVMTypeMap {
  LLVMContext &TheContext;

public:
  /// @brief Constructor.
  explicit LLVMTypeMap(LLVMContext &Context) : TheContext(Context) {}

  /// @brief Convert a BasicTypeKind to a llvm type.
  /// 1. Void => void.
  /// 2. Int => i32.
  /// 3. Character => i8.
  Type *getType(BasicTypeKind Type) const;

  /// Convert different basic types.
  Type *getCharType() const { return getType(BasicTypeKind::Character); }
  Type *getIntType() const { return getType(BasicTypeKind::Int); }

  /// @brief Return the type for bool.
  /// getBoolType() is a shortcut for llvm's i1.
  Type *getBoolType() const { return Type::getInt1Ty(TheContext); }

  /// @brief Convert an ArrayType.
  Type *getType(const ArrayType &A) const;

  /// @brief Convert a VarType.
  Type *getType(const VarType &V) const { return getType(V.getType()); }

  /// @brief Convert a ConstType.
  Type *getType(const ConstType &C) const { return getType(C.getType()); }

  /// @brief Convert a FuncType.
  FunctionType *getType(const FuncType &F) const;

  /// @brief Convert a FuncDef.
  FunctionType *getTypeFromFuncDef(FuncDef *FD) const {
    return getType(FuncType(FD));
  }

  /// @brief Convert a VarDecl.
  Type *getTypeFromVarDecl(VarDecl *VD) const;

  /// @brief Convert a ConstDecl.
  Type *getTypeFromConstDecl(ConstDecl *CD) const {
    return getType(CD->getType());
  }
};

} // namespace simplecc
#endif // SIMPLECC_LLVM_LLVMTYPEMAP_H