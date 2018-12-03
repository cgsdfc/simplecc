#include "ASTGraph.h"
#include "Visitor.h"
#include "Print.h"

#include <llvm/ADT/iterator.h>
#include <stack>

namespace simplecompiler {

enum class AstKind {
  Program,
  Decl,
  Stmt,
  Arg,
  Expr,
};

/// Automatically extracted AstKind
template <typename AstT> struct AstTraits {
  static const AstKind Kind = AstT::UnknownAstKindError;
};

#define DEFINE_AST_TRAITS(NAME) \
template <> struct AstTraits<NAME> { \
  static const AstKind Kind = AstKind::NAME; \
};

DEFINE_AST_TRAITS(Program)
DEFINE_AST_TRAITS(Decl)
DEFINE_AST_TRAITS(Stmt)
DEFINE_AST_TRAITS(Arg)
DEFINE_AST_TRAITS(Expr)

class ASTRef {
  AST *Ref;
  AstKind Kind;

public:

  template <typename AstT>
    ASTRef(AstT *Ptr): Ref(Ptr), Kind(AstTraits<AstT>::Kind) {}

  ASTRef() : Ref(nullptr) {}

  AstKind getKind() const { return Kind; }

  bool operator==(const ASTRef &O) const {
    return Ref == O.Ref;
  }

  bool empty() const {
    return nullptr == Ref;
  }

  operator bool() const { return !empty(); }

  /// Checked downcast
  template <typename AstT>
    AstT *get() const {
      if (empty()) return nullptr;
      if (AstTraits<AstT>::Kind != getKind())
        return nullptr;
      return static_cast<AstT*>(Ref);
    }

  const char *getClassName() const {
    assert(Ref);
    return Ref->GetClassName();
  }

};

class ASTIteratorImpl : public ChildrenVisitor<ASTIteratorImpl> {
  std::stack<ASTRef> Stack;

  void AddChildren(ASTRef R) {
    if (auto D = R.get<Decl>())
      return VisitorBase::visitDecl<void>(D);
    if (auto S = R.get<Stmt>())
      return VisitorBase::visitStmt<void>(S);
    if (auto E = R.get<Expr>())
      return VisitorBase::visitExpr<void>(E);
    if (auto P = R.get<Program>())
      return ChildrenVisitor::visitProgram(P);
    // Arg has no children
    auto A = R.get<Arg>();
    assert(A);
    return visitArg(A);
  }

  template <typename AstT>
    void AddChild(AstT *Ptr) {
      Stack.emplace(Ptr);
    }

public:
  /// For ChildrenVisitor to hook in
  void visitExpr(Expr *E) { AddChild(E); }
  void visitDecl(Decl *D) { AddChild(D); }
  void visitStmt(Stmt *S) { AddChild(S); }

public:
  ASTIteratorImpl(Program *P): Stack() {
    Stack.push(P);
  }

  ASTIteratorImpl(): Stack() {}

  ASTRef getNext() {
    if (Stack.empty()) {
      return ASTRef();
    }
    auto TOS = Stack.top();
    Stack.pop();
    AddChildren(TOS);
    return TOS;
  }

};

class AstIterator:
  private ASTIteratorImpl,
  public llvm::iterator_facade_base<AstIterator, std::forward_iterator_tag, ASTRef> {

public:
  AstIterator(Program *P) : ASTIteratorImpl(P) { operator++(); }
  AstIterator() :ASTIteratorImpl(), Ref() {}

  ASTRef operator*() const { return Ref; }
  bool operator==(const AstIterator &O) const { return O.Ref == Ref; }

  AstIterator &operator++() {
    Ref = getNext();
    return *this;
  }

private:
  ASTRef Ref;
};

static inline AstIterator AstNodeBegin(Program *P) { return AstIterator(P); }
static inline AstIterator AstNodeEnd() { return AstIterator(); }

static inline llvm::iterator_range<AstIterator> AstNodes(Program *P) {
  return llvm::make_range(AstNodeBegin(P), AstNodeEnd());
}

void PrintAllAstNodes(Program *P, std::ostream &os) {
  for (auto AR : AstNodes(P)) {
    Print(os, AR.getClassName());
  }
}

}
