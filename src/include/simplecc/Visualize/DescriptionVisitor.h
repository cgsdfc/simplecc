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

#ifndef SIMPLECC_VISUALIZE_DESCRIPTIONVISITOR_H
#define SIMPLECC_VISUALIZE_DESCRIPTIONVISITOR_H
#include "simplecc/AST/VisitorBase.h"

namespace simplecc {
class ASTNode;

/// This class generates a description for each AST node.
class DescriptionVisitor : public VisitorBase<DescriptionVisitor> {
  friend VisitorBase;
  /// VisitorBase boilerplate code.
  std::string visitDecl(DeclAST *D) {
    return VisitorBase::visitDecl<std::string>(D);
  }
  std::string visitExpr(ExprAST *E) {
    return VisitorBase::visitExpr<std::string>(E);
  }
  std::string visitStmt(StmtAST *S) {
    return VisitorBase::visitStmt<std::string>(S);
  }

  std::string visitProgram(ProgramAST *) { return ""; }
  std::string visitConstDecl(ConstDecl *CD);
  std::string visitVarDecl(VarDecl *VD);
  std::string visitFuncDef(FuncDef *FD);
  std::string visitArgDecl(ArgDecl *A);

  std::string visitBoolOp(BoolOpExpr *BO) { return ""; }
  std::string visitParenExpr(ParenExpr *PE) { return "()"; }
  std::string visitNum(NumExpr *N);
  std::string visitChar(CharExpr *C);
  std::string visitStr(StrExpr *S);
  std::string visitCall(CallExpr *C) { return C->getCallee(); }
  std::string visitName(NameExpr *N) { return N->getName(); }
  std::string visitSubscript(SubscriptExpr *SB) { return SB->getArrayName(); }

  std::string visitBinOp(BinOpExpr *BO) {
    return CStringFromOperatorKind(BO->getOp());
  }
  std::string visitUnaryOp(UnaryOpExpr *UO) {
    return CStringFromUnaryopKind(UO->getOp());
  }

  std::string visitExprStmt(ExprStmt *ES) { return ""; }
  std::string visitFor(ForStmt *) { return ""; }
  std::string visitIf(IfStmt *) { return ""; }
  std::string visitWhile(WhileStmt *) { return ""; }
  std::string visitReturn(ReturnStmt *) { return ""; }
  std::string visitRead(ReadStmt *) { return "scanf"; }
  std::string visitWrite(WriteStmt *) { return "printf"; }
  std::string visitAssign(AssignStmt *) { return "="; }

public:
  DescriptionVisitor() = default;
  /// Return a descriptive string for AR.
  std::string makeDescription(const ASTNode &AN);
};

} // namespace simplecc
#endif // SIMPLECC_VISUALIZE_DESCRIPTIONVISITOR_H