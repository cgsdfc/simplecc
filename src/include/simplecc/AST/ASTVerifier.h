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

#ifndef SIMPLECC_AST_ASTVERIFIER_H
#define SIMPLECC_AST_ASTVERIFIER_H
#include "simplecc/AST/ChildrenVisitor.h"
#include "simplecc/Support/ErrorManager.h"

namespace simplecc {
/// @brief ASTVerifier verifies that various AST nodes can only be of a fixed range of subclasses.
/// These conditions cannot be enforced by a C++ compiler although the much the better.
/// Example is the Target of an AssignStmt can only be a NameExpr or SubscriptExpr, not other things.
/// The constrains are imposed by the language specification.
class ASTVerifier : ChildrenVisitor<ASTVerifier> {
  /// Helper to check a condition.
  void AssertThat(bool Predicate, const char *ErrMsg);

  void visitWrite(WriteStmt *WR);
  void visitAssign(AssignStmt *A);
  void visitBoolOp(BoolOpExpr *B);
  void visitExprStmt(ExprStmt *ES);
  void visitConstDecl(ConstDecl *CD);
  void visitFor(ForStmt *F);
  void visitWhile(WhileStmt *W);
  void visitIf(IfStmt *I);
  void visitFuncDef(FuncDef *FD);
  void visitProgram(ProgramAST *P);

public:
  ASTVerifier() = default;
  ~ASTVerifier() = default;
  /// Check a program.
  bool Check(ProgramAST *P);
private:
  friend ChildrenVisitor;
  friend VisitorBase;
  ErrorManager EM;
};
} // namespace simplecc
#endif // SIMPLECC_AST_ASTVERIFIER_H