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

#ifndef SIMPLECC_VISUALIZE_CHILDRENCOLLECTOR_H
#define SIMPLECC_VISUALIZE_CHILDRENCOLLECTOR_H
#include "simplecc/AST/ChildrenVisitor.h"
#include "simplecc/Visualize/ASTGraph.h"

namespace simplecc {
class ASTNode;

/// Collect children of an ASTNode into a vector for later use.
class ChildrenCollector : ChildrenVisitor<ChildrenCollector> {
  /// Add a child.
  void AddChild(AST *Ptr) {
    ASTNode *AR = Parent->getNodeOrCreate(Ptr);
    Children.push_back(AR);
  }

  /// For ChildrenVisitor to hook in
  void visitExpr(ExprAST *E) { AddChild(E); }
  void visitDecl(DeclAST *D) { AddChild(D); }
  void visitStmt(StmtAST *S) { AddChild(S); }

public:
  ChildrenCollector(std::vector<ASTNode *> &Vec, ASTGraph *G)
      : Children(Vec), Parent(G) {}

  /// Call this to do the collecting. Otherwise, nothing will happen.
  void Collect(const ASTNode &R) {
    Children.clear();
    visitAST(R.get());
  }

private:
  friend ChildrenVisitor;
  friend VisitorBase;
  /// Keep a reference to the output vector.
  std::vector<ASTNode *> &Children;
  /// Used to construct an ASTNode.
  ASTGraph *Parent;
};
} // namespace simplecc

#endif // SIMPLECC_VISUALIZE_CHILDRENCOLLECTOR_H