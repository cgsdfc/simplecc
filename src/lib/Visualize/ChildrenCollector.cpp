#include "simplecc/Visualize/ChildrenCollector.h"
#include "simplecc/Visualize/AstRef.h"

using namespace simplecc;

void ChildrenCollector::Collect(const simplecc::AstRef &R) {
  Children.clear();
  if (auto D = R.get<Decl>())
    return ChildrenVisitor::visitDecl(D);
  if (auto S = R.get<Stmt>())
    return ChildrenVisitor::visitStmt(S);
  if (auto E = R.get<Expr>())
    return ChildrenVisitor::visitExpr(E);
  if (auto P = R.get<Program>())
    return ChildrenVisitor::visitProgram(P);
  // ArgDecl has no children
}
