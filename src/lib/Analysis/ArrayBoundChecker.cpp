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

#include "simplecc/Analysis/ArrayBoundChecker.h"

using namespace simplecc;

void ArrayBoundChecker::visitSubscript(SubscriptExpr *SB) {
  auto Entry = getSymbolEntry(SB->getArrayName());
  if (!Entry.IsArray()) {
    return;
  }
  ArrayType AT(Entry.AsArray());
  std::pair<bool, int> Idx = getIndex(SB->getIndex());
  if (!Idx.first)
    return;
  int Val = Idx.second;
  if (Val < 0 || Val >= AT.getSize()) {
    Error(SB->getLocation(), "array index out of bound:", Val);
  }
}

std::pair<bool, int> ArrayBoundChecker::getIndex(ExprAST *E) const {
  std::pair<bool, int> False(false, 0);
  // Case-1: NumExpr.
  if (IsInstance<NumExpr>(E)) {
    return std::make_pair(true, static_cast<NumExpr *>(E)->getNum());
  }

  // Case-2: UnaryOpExpr[NumExpr].
  if (IsInstance<UnaryOpExpr>(E) &&
      IsInstance<NumExpr>(static_cast<UnaryOpExpr *>(E)->getOperand())) {
    auto Extract = [](UnaryOpExpr *U) {
      auto Val = static_cast<NumExpr *>(U->getOperand())->getNum();
      return U->getOp() == UnaryOpKind::USub ? -Val : Val;
    };
    return std::make_pair(true, Extract(static_cast<UnaryOpExpr *>(E)));
  }

  // Case-3: const int NameConstant = 1;
  if (!IsInstance<NameExpr>(E))
    return False;

  auto N = static_cast<NameExpr *>(E);
  auto Entry = getSymbolEntry(N->getName());
  if (!Entry.IsConstant())
    return False;

  ConstType CT(Entry.AsConstant());
  if (CT.getType() != BasicTypeKind::Int)
    return False;

  return std::make_pair(true, CT.getValue());
}