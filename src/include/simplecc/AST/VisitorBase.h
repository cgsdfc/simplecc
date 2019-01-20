#ifndef SIMPLECC_AST_VISITORBASE_H
#define SIMPLECC_AST_VISITORBASE_H
#include "simplecc/AST/AST.h"
#include "simplecc/Support/Casting.h"

namespace simplecc {

/// @brief VisitorBase is a CRTP mixin that provides dispatch over polymorphic AST nodes.
/// It implements the visitor methods for all abstract AST classes and calls the
/// visitor methods of concrete AST classes from Derived. Derived of it must implement
/// visitor methods for all concrete AST classes or the code won't compile.
///
/// The visitor methods have a template parameter for their return type, which default to ``void``.
/// This flexibility comes with some inconvenience. If you just return void, the usage is as normal.
/// But if you return some other thing, the template parameter must be provided explicitly or you can
/// wrap the explicitly parameterized method in a inline method. The latter approach is encouraged since
/// it makes code less repeated.
/// \tparam Derived Subclass that wants to visit the AST and want to implement every single visitor methods.
template <typename Derived> class VisitorBase {
public:
  /// Visit any DeclAST node.
  template <typename RetTy = void> RetTy visitDecl(DeclAST *D);
  /// Visit any StmtAST node.
  template <typename RetTy = void> RetTy visitStmt(StmtAST *S);
  /// Visit any ExprAST node.
  template <typename RetTy = void> RetTy visitExpr(ExprAST *E);
  /// Visit any AST node.
  template <typename RetTy = void> RetTy visitAST(AST *A);
};

// Methods of VisitorBase.
template <typename Derived>
template <typename RetTy>
RetTy VisitorBase<Derived>::visitDecl(DeclAST *D) {
  if (auto x = subclass_cast<ConstDecl>(D)) {
    return static_cast<Derived *>(this)->visitConstDecl(x);
  }
  if (auto x = subclass_cast<VarDecl>(D)) {
    return static_cast<Derived *>(this)->visitVarDecl(x);
  }
  if (auto x = subclass_cast<FuncDef>(D)) {
    return static_cast<Derived *>(this)->visitFuncDef(x);
  }
  if (auto x = subclass_cast<ArgDecl>(D)) {
    return static_cast<Derived *>(this)->visitArgDecl(x);
  }
  assert(false && "Unhandled DeclAST subclasses");
}

template <typename Derived>
template <typename RetTy>
RetTy VisitorBase<Derived>::visitStmt(StmtAST *S) {
  if (auto x = subclass_cast<ReadStmt>(S)) {
    return static_cast<Derived *>(this)->visitRead(x);
  }
  if (auto x = subclass_cast<WriteStmt>(S)) {
    return static_cast<Derived *>(this)->visitWrite(x);
  }
  if (auto x = subclass_cast<AssignStmt>(S)) {
    return static_cast<Derived *>(this)->visitAssign(x);
  }
  if (auto x = subclass_cast<ForStmt>(S)) {
    return static_cast<Derived *>(this)->visitFor(x);
  }
  if (auto x = subclass_cast<WhileStmt>(S)) {
    return static_cast<Derived *>(this)->visitWhile(x);
  }
  if (auto x = subclass_cast<ReturnStmt>(S)) {
    return static_cast<Derived *>(this)->visitReturn(x);
  }
  if (auto x = subclass_cast<IfStmt>(S)) {
    return static_cast<Derived *>(this)->visitIf(x);
  }
  if (auto x = subclass_cast<ExprStmt>(S)) {
    return static_cast<Derived *>(this)->visitExprStmt(x);
  }
  assert(false && "Unhandled StmtAST subclasses");
}

template <typename Derived>
template <typename RetTy>
RetTy VisitorBase<Derived>::visitExpr(ExprAST *E) {
  if (auto x = subclass_cast<BinOpExpr>(E)) {
    return static_cast<Derived *>(this)->visitBinOp(x);
  }
  if (auto x = subclass_cast<ParenExpr>(E)) {
    return static_cast<Derived *>(this)->visitParenExpr(x);
  }
  if (auto x = subclass_cast<BoolOpExpr>(E)) {
    return static_cast<Derived *>(this)->visitBoolOp(x);
  }
  if (auto x = subclass_cast<UnaryOpExpr>(E)) {
    return static_cast<Derived *>(this)->visitUnaryOp(x);
  }
  if (auto x = subclass_cast<CallExpr>(E)) {
    return static_cast<Derived *>(this)->visitCall(x);
  }
  if (auto x = subclass_cast<NumExpr>(E)) {
    return static_cast<Derived *>(this)->visitNum(x);
  }
  if (auto x = subclass_cast<StrExpr>(E)) {
    return static_cast<Derived *>(this)->visitStr(x);
  }
  if (auto x = subclass_cast<CharExpr>(E)) {
    return static_cast<Derived *>(this)->visitChar(x);
  }
  if (auto x = subclass_cast<SubscriptExpr>(E)) {
    return static_cast<Derived *>(this)->visitSubscript(x);
  }
  if (auto x = subclass_cast<NameExpr>(E)) {
    return static_cast<Derived *>(this)->visitName(x);
  }
  assert(false && "Unhandled ExprAST subclasses");
}

template <typename Derived>
template <typename RetTy>
RetTy VisitorBase<Derived>::visitAST(AST *A) {
  if (auto x = subclass_cast<ProgramAST>(A))
    return static_cast<Derived *>(this)->visitProgram(x);
  if (auto x = subclass_cast<DeclAST>(A))
    return visitDecl<RetTy>(x);
  if (auto x = subclass_cast<StmtAST>(A))
    return visitStmt<RetTy>(x);
  if (auto x = subclass_cast<ExprAST>(A))
    return visitExpr<RetTy>(x);
  assert(false && "Unhandled AST subclasses");
}
} // namespace simplecc
#endif //SIMPLECC_AST_VISITORBASE_H
