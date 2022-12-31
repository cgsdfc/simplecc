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

#ifndef SIMPLECC_LLVM_LLVMVALUEMAP_H
#define SIMPLECC_LLVM_LLVMVALUEMAP_H
#include "simplecc/LLVM/LLVMTypeMap.h"
#include <llvm/IR/Constants.h>

namespace simplecc {
using llvm::Constant;
using llvm::ConstantInt;
using llvm::Value;

/// @brief LLVMValueMap converts values of different types (mostly constants)
/// to their LLVM counterparts.
class LLVMValueMap : public LLVMTypeMap {
  llvm::Module &TheModule;

public:
  LLVMValueMap(llvm::Module &M, LLVMContext &Context)
      : LLVMTypeMap(Context), TheModule(M) {}

  /// @brief Return a Value for an int.
  Constant *getInt(int N) const;

  /// @brief Return a Value for a char.
  Constant *getChar(int C) const;

  /// @brief Return a Value for a constant expression.
  /// Require \param E must be either a NumExpr or CharExpr.
  Constant *getConstantFromExpr(ExprAST *E) const;

  /// @brief Return a Value for global initializer of a variable.
  Constant *getGlobalInitializer(VarDecl *VD);
};

} // namespace simplecc
#endif // SIMPLECC_LLVM_LLVMVALUEMAP_H
