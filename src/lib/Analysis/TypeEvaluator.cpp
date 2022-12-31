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

#include "simplecc/Analysis/TypeEvaluator.h"
#include <cassert>

using namespace simplecc;

BasicTypeKind TypeEvaluator::visitSubscript(SubscriptExpr *S) {
  auto Entry = TheLocal[S->getArrayName()];
  assert(Entry.IsArray() && "invalid access to non array");
  return Entry.AsArray().getElementType();
}

BasicTypeKind TypeEvaluator::visitName(NameExpr *N) {
  // TODO: fix the mixture of SymbolEntry and type judgement.
  auto Entry = TheLocal[N->getName()];
  if (Entry.IsVariable())
    return Entry.AsVariable().getType();
  if (Entry.IsConstant())
    return Entry.AsConstant().getType();
  assert(false && "Bad type for NameExpr");
}

BasicTypeKind TypeEvaluator::visitCall(CallExpr *C) {
  auto Entry = TheLocal[C->getCallee()];
  assert(Entry.IsFunction() && "invalid access to non function");
  return Entry.AsFunction().getReturnType();
}

BasicTypeKind TypeEvaluator::getExprType(const ExprAST *E) const {
  return const_cast<TypeEvaluator *>(this)->visitExpr(const_cast<ExprAST *>(E));
}
