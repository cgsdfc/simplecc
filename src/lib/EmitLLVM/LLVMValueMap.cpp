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