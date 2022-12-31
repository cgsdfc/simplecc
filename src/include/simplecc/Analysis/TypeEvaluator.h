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

#ifndef SIMPLECC_ANALYSIS_TYPEEVALUATOR_H
#define SIMPLECC_ANALYSIS_TYPEEVALUATOR_H
#include "simplecc/AST/VisitorBase.h"
#include "simplecc/Analysis/SymbolTable.h"
#include <cassert>

namespace simplecc {
/// @brief TypeEvaluator evaluates the result type of an expression.
/// TypeEvaluator implements the simple rules of the type system.
/// Evaluation of some classes of node requires a LocalSymbolTable so
/// client needs to pass one to its constructor.
/// For example:
/// @code
/// int array[10];
/// typeof(array[2]) is int.
///
/// int var;
/// typeof(var) is int.
///
/// const char i = 'c';
/// typeof(i) is char.
/// typeof(('a')) is int.
///
/// void foo(int i) {}
/// typeof(foo(0)) is void.
/// @endcode
class TypeEvaluator : VisitorBase<TypeEvaluator> {
  /// @brief Instantiate a VisitorBase method.
  BasicTypeKind visitExpr(const ExprAST *E) {
    return VisitorBase::visitExpr<BasicTypeKind>(const_cast<ExprAST *>(E));
  }

  BasicTypeKind visitStr(StrExpr *S) {
    // string literal is not part of the type system.
    assert(false && "StrExpr has no type");
    return BasicTypeKind::Void;
  }

  BasicTypeKind visitNum(NumExpr *N) {
    // NumExpr is int.
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitChar(CharExpr *C) {
    // CharExpr is int.
    return BasicTypeKind::Character;
  }

  BasicTypeKind visitParenExpr(ParenExpr *P) {
    // putting parentheses around an expression cast its type to int.
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitBinOp(BinOpExpr *B) {
    // Result of a BinOpExpr is int.
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitBoolOp(BoolOpExpr *B) {
    // Result of a BoolOpExpr is int.
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitUnaryOp(UnaryOpExpr *U) {
    // Result of a UnaryOpExpr is int.
    return BasicTypeKind::Int;
  }

  /// @brief Return the type of this name.
  /// @param N must has Load context and is a variable or constant.
  /// typeof(NameExpr) depends on a LocalSymbolTable.
  BasicTypeKind visitName(NameExpr *N);

  /// @brief
  /// @param S must has Load context and is an array.
  /// typeof(SubscriptExpr) depends on a LocalSymbolTable.
  BasicTypeKind visitSubscript(SubscriptExpr *S);

  /// @brief
  /// @param C must be a function.
  /// typeof(CallExpr) depends on a LocalSymbolTable.
  BasicTypeKind visitCall(CallExpr *C);

public:
  /// Construct a TypeEvaluator with a \param Local.
  explicit TypeEvaluator(LocalSymbolTable Local) : TheLocal(Local) {}
  ~TypeEvaluator() = default;

  /// @brief Return the type of an ExprAST.
  BasicTypeKind getExprType(const ExprAST *E) const;

  /// @brief getExprType is a helper that saves the construction of a TypeEvaluator.
  static BasicTypeKind getExprType(const ExprAST *E, LocalSymbolTable Local) {
    return TypeEvaluator(Local).getExprType(E);
  }

private:
  friend VisitorBase;
  LocalSymbolTable TheLocal;
};

} // namespace simplecc
#endif // SIMPLECC_ANALYSIS_TYPEEVALUATOR_H