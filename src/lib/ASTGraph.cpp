#include "simplecc/ASTGraph.h"
#include "simplecc/Print.h"
#include "simplecc/TokenInfo.h" // for Location
#include "simplecc/Visitor.h"

#include <llvm/ADT/GraphTraits.h>
#include <llvm/ADT/iterator.h>
#include <llvm/Support/GraphWriter.h>

#include <algorithm>
#include <iostream>
#include <iterator> // for forward_iterator_tag
#include <memory> // for make_unique
#include <sstream>
#include <stack>
#include <string>
#include <utility>     // for move
#include <vector>

namespace simplecc {
/// Collect children of an AstRef into a vector for later use.
class ChildrenCollector : ChildrenVisitor<ChildrenCollector> {
  friend class ChildrenVisitor<ChildrenCollector>;
  friend class VisitorBase<ChildrenCollector>;

  /// Keep a reference to the output vector.
  std::vector<AstRef *> &Children;
  /// Used to construct an AstRef.
  AstGraph *Parent;

  /// Add a child.
  template<typename AstT> void AddChild(AstT *Ptr);

  /// For ChildrenVisitor to hook in
  void visitExpr(Expr *E) { AddChild(E); }
  void visitDecl(Decl *D) { AddChild(D); }
  void visitStmt(Stmt *S) { AddChild(S); }

public:
  /// The type used to iterate all the children of a node.
  using ChildrenIterator = std::vector<AstRef *>::const_iterator;

  ChildrenCollector(std::vector<AstRef *> &Vec, AstGraph *G)
      : Children(Vec), Parent(G) {}

  /// Call this to do the collecting. Otherwise, nothing will happen.
  void Collect(const AstRef &R);
};

void ChildrenCollector::Collect(const simplecc::AstRef &R) {
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

const std::vector<AstRef *> &AstGraph::getEdgeOrCreate(const AstRef &R) {
  auto iter = Edges.find(R.get());
  if (iter != Edges.end())
    return iter->second;

  std::vector<AstRef *> E;
  ChildrenCollector CC(E, this);
  CC.Collect(R);

  auto Result = Edges.emplace(R.get(), std::move(E));
  assert(Result.second && "Emplace must succeed");
  return Result.first->second;
}

/// Add a child.
template<typename AstT> void ChildrenCollector::AddChild(AstT *Ptr) {
  AstRef *AR = Parent->getNodeOrCreate(Ptr);
  Children.push_back(AR);
}

/// Print all ast nodes from a root.
void PrintAllAstNodes(Program *P, std::ostream &os) {
  AstGraph Graph(P);
  for (auto AR : Graph.nodes()) {
    Print(os, AR->getClassName(), AR->getLocation());
  }
}

/// Write an AstGraph to dot format.
void WriteASTGraph(Program *P, llvm::raw_ostream &O) {
  AstGraph Graph(P);
  llvm::WriteGraph(O, Graph);
}

} // namespace simplecc

namespace simplecc {

/// This class generates a description for each AST node.
class DescriptionVisitor : public VisitorBase<DescriptionVisitor> {
public:
  /// Return a descriptive string for AR.
  String makeDescription(const AstRef &AR) {
    switch (AR.getKind()) {
#define HANDLE_AST_TYPE(NAME)                                                \
  case AstKind::NAME:                                                          \
    return visit##NAME(static_cast<NAME *>(AR.get()));
    HANDLE_AST_TYPE(Decl)
    HANDLE_AST_TYPE(Expr)
    HANDLE_AST_TYPE(Stmt)
#undef HANDLE_AST_TYPE
    default:return "";
    }
  }

  /// VisitorBase boilderplate code.
  String visitDecl(Decl *D) { return VisitorBase::visitDecl<String>(D); }
  String visitExpr(Expr *E) { return VisitorBase::visitExpr<String>(E); }
  String visitStmt(Stmt *S) { return VisitorBase::visitStmt<String>(S); }

  String visitConstDecl(ConstDecl *CD) {
    std::ostringstream O;
    /// lambda to extract the numeric value of a Num or Char.
    auto MakeCV = [](Expr *E) {
      if (auto x = subclass_cast<Char>(E))
        return x->getC();
      if (auto x = subclass_cast<Num>(E))
        return x->getN();
      assert(false && "Unknown Expr class");
    };

    O << "const " << CStringFromBasicTypeKind(CD->getType()) << " "
      << CD->getName() << " = " << MakeCV(CD->getValue());
    return O.str();
  }

  String visitVarDecl(VarDecl *VD) {
    std::ostringstream O;
    O << CStringFromBasicTypeKind(VD->getType()) << " " << VD->getName();
    if (VD->getIsArray()) {
      O << "[" << VD->getSize() << "]";
    }
    return O.str();
  }

  String visitFuncDef(FuncDef *FD) {
    std::ostringstream O;
    Print(O, CStringFromBasicTypeKind(FD->getReturnType()), FD->getName());
    return O.str();
  }

  String visitArgDecl(ArgDecl *A) {
    std::ostringstream O;
    Print(O, CStringFromBasicTypeKind(A->getType()), A->getName());
    return O.str();
  }

  String visitBinOp(BinOp *BO) { return CStringFromOperatorKind(BO->getOp()); }

  String visitUnaryOp(UnaryOp *UO) {
    return CStringFromUnaryopKind(UO->getOp());
  }

  String visitBoolOp(BoolOp *BO) { return ""; }

  String visitParenExpr(ParenExpr *PE) { return "()"; }

  String visitName(Name *N) { return N->getId(); }

  String visitNum(Num *N) {
    std::ostringstream O;
    O << N->getN();
    return O.str();
  }

  String visitChar(Char *C) {
    std::ostringstream O;
    O << "'" << C->getC() << "'";
    return O.str();
  }

  String visitStr(Str *S) {
    std::ostringstream O;
    O << S->getS();
    return O.str();
  }

  String visitCall(Call *C) { return C->getFunc(); }

  String visitRead(Read *) { return "scanf"; }

  String visitWrite(Write *) { return "printf"; }

  String visitAssign(Assign *) { return "="; }

  String visitSubscript(Subscript *SB) { return SB->getName(); }

  String visitExprStmt(ExprStmt *ES) { return ""; }
  String visitFor(For *) { return ""; }
  String visitIf(If *) { return ""; }
  String visitWhile(While *) { return ""; }
  String visitReturn(Return *) { return ""; }
};

} // namespace simplecc

namespace llvm {
using simplecc::AstGraph;
using simplecc::AstRef;

/// Specialized GraphTraits for AstGraph
template<> struct GraphTraits<AstGraph> {
  using NodeRef = AstRef *;
  using nodes_iterator = AstGraph::NodeIterator;
  using ChildIteratorType = AstGraph::ChildIteratorType;

  static nodes_iterator nodes_begin(const AstGraph &G) {
    return G.nodes_begin();
  }

  static nodes_iterator nodes_end(const AstGraph &G) { return G.nodes_end(); }

  static ChildIteratorType child_begin(NodeRef N) {
    return (*N).getParent()->child_begin(*N);
  }

  static ChildIteratorType child_end(NodeRef N) {
    return (*N).getParent()->child_end(*N);
  }
};

/// Specialized DOTGraphTraits for AstGraph.
template<> struct DOTGraphTraits<AstGraph> : DefaultDOTGraphTraits {
  DOTGraphTraits(bool simple = false) : DefaultDOTGraphTraits(simple) {}

  static std::string getGraphName(const AstGraph &) {
    return "Abstract Syntax Tree";
  }

  std::string getNodeLabel(const void *NodeRef, const AstGraph &) {
    auto N = static_cast<AstRef *>(const_cast<void *>(NodeRef));
    return N->getClassName();
  }

  static std::string getNodeDescription(const void *NodeRef, const AstGraph &) {
    auto N = static_cast<AstRef *>(const_cast<void *>(NodeRef));
    simplecc::DescriptionVisitor DV;
    return DV.makeDescription(*N);
  }
};

} // namespace llvm
