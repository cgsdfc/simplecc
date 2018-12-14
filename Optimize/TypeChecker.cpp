#include "TypeChecker.h"
#include "ErrorManager.h"
#include "SymbolTable.h"
#include "Visitor.h"

#include <algorithm>

namespace simplecompiler {
// Check type for a function
class TypeCheker : ChildrenVisitor<TypeCheker> {

  void visitRead(Read *RD) {
    for (auto E : RD->getNames()) {
      visitExpr(E); // Collect type info.
      auto N = static_cast<Name *>(E);
      const auto &entry = TheLocalTable[N->getId()];
      if (!entry.IsVariable()) {
        EM.TypeError(N->getLoc(), "cannot use scanf() on object of type",
                     entry.GetTypeName());
      }
    }
  }

  void visitWrite(Write *WR) {
    if (WR->getValue()) {
      CheckExprOperand(WR->getValue());
    }
  }

  void visitReturn(Return *R) {
    auto ActuallyReturn =
        R->getValue() ? visitExpr(R->getValue()) : BasicTypeKind::Void;
    auto ShouldReturn = TheFuncDef->getReturnType();

    // order a strict match
    if (ShouldReturn != ActuallyReturn) {
      EM.TypeError(R->getLoc(), "return type mismatched:", "function",
                   Quote(TheFuncDef->getName()), "must return",
                   CStringFromBasicTypeKind(ShouldReturn), "not",
                   CStringFromBasicTypeKind(ActuallyReturn));
    }
  }

  void visitAssign(Assign *A) {
    int errs = EM.GetErrorCount();
    auto value = CheckExprOperand(A->getValue());
    auto target = visitExpr(A->getTarget());

    if (EM.IsOk(errs) && target != value) {
      EM.TypeError(A->getLoc(), "type mismatched in assignment:",
                   CStringFromBasicTypeKind(target), "=",
                   CStringFromBasicTypeKind(value));
    }
  }

  void visitExprStmt(ExprStmt *ES) {
    auto call = subclass_cast<Call>(ES->getValue());
    assert(call && "value of ExprStmt must be a Call");
    visitCall(call);
  }

  // check the operand of BoolOp, restrict to int
  void CheckBoolOpOperand(Expr *E) {
    auto msg = "operands of condition must be of type int";
    auto errs = EM.GetErrorCount();
    auto type = visitExpr(E);
    if (EM.IsOk(errs) && type != BasicTypeKind::Int) {
      EM.TypeError(E->getLoc(), msg);
    }
  }

  BasicTypeKind visitBoolOp(BoolOp *B) {
    if (B->getHasCmpop()) {
      auto x = static_cast<BinOp *>(B->getValue());
      CheckBoolOpOperand(x->getLeft());
      CheckBoolOpOperand(x->getRight());
    } else {
      CheckBoolOpOperand(B->getValue());
    }
    return BasicTypeKind::Int;
  }

  // check the operand of Expr, restrict to NOT void
  BasicTypeKind CheckExprOperand(Expr *E) {
    auto msg = "void value cannot be used in an expression";
    auto errs = EM.GetErrorCount();
    auto type = visitExpr(E);
    if (EM.IsOk(errs) && type == BasicTypeKind::Void) {
      EM.TypeError(E->getLoc(), msg);
    }
    return type;
  }

  BasicTypeKind visitBinOp(BinOp *B) {
    CheckExprOperand(B->getLeft());
    CheckExprOperand(B->getRight());
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitUnaryOp(UnaryOp *U) {
    CheckExprOperand(U->getOperand());
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitParenExpr(ParenExpr *PE) {
    CheckExprOperand(PE->getValue());
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitCall(Call *C) {
    const auto &entry = TheLocalTable[C->getFunc()];
    if (!entry.IsFunction()) {
      EM.TypeError(C->getLoc(), "object of type", entry.GetTypeName(),
                   "cannot be called as a function");
      return BasicTypeKind::Void;
    }

    auto fun_type = entry.AsFunction();
    auto formal_args_len = fun_type.GetArgCount();
    auto actual_args_len = C->args.size();
    if (formal_args_len != actual_args_len) {
      EM.TypeError(C->getLoc(), "function", Quote(C->getFunc()), "expects",
                   formal_args_len, "arguments, got", actual_args_len);
    }

    // check args
    auto len = std::min(formal_args_len, actual_args_len);
    for (int i = 0; i < len; i++) {
      auto actual = visitExpr(C->getArgs()[i]);
      auto formal = fun_type.GetArgTypeAt(i);
      if (actual != formal) {
        EM.TypeError(C->getArgs()[i]->getLoc(), "argument", i + 1,
                     "of function", Quote(C->getFunc()), "must be",
                     CStringFromBasicTypeKind(formal), ", not",
                     CStringFromBasicTypeKind(actual));
      }
    }
    return fun_type.GetReturnType();
  }

  BasicTypeKind visitSubscript(Subscript *SB) {
    const auto &entry = TheLocalTable[SB->getName()];
    if (!entry.IsArray()) {
      EM.TypeError(SB->getLoc(), "object of type", entry.GetTypeName(),
                   "cannot be subscripted as an array");
      return BasicTypeKind::Void;
    }

    int errs = EM.GetErrorCount();
    auto index = visitExpr(SB->getIndex());
    if (EM.IsOk(errs) && index != BasicTypeKind::Int) {
      EM.TypeError(SB->getLoc(), "type of array index must be int");
    }
    return entry.AsArray().GetElementType();
  }

  BasicTypeKind visitName(Name *N) {
    const auto &entry = TheLocalTable[N->getId()];
    if (N->getCtx() == ExprContextKind::Load && entry.IsArray()) {
      EM.TypeError(N->getLoc(), "object of type", entry.GetTypeName(),
                   "cannot be used in an expression");
      return BasicTypeKind::Void;
    }
    if (N->getCtx() == ExprContextKind::Store && !entry.IsVariable()) {
      EM.TypeError(N->getLoc(), "object of type", entry.GetTypeName(),
                   "cannot be assigned to");
      return BasicTypeKind::Void;
    }
    if (entry.IsConstant())
      return entry.AsConstant().GetType();
    return entry.AsVariable().GetType();
  }

  // not actually used, for instantiation only
  BasicTypeKind visitStr(Str *) {
    assert(false && "TypeCheker::visitStr() shall not be called");
    return BasicTypeKind::Void;
  }

  // Return the type of evaluating the expression
  BasicTypeKind visitExpr(Expr *E) {
    auto type = VisitorBase::visitExpr<BasicTypeKind>(E);
    TheTable->setExprType(E, type);
    return type;
  }

  void visitFuncDef(FuncDef *FD) {
    assert(TheTable);
    setFuncDef(FD);
    setLocalTable(TheTable->getLocalTable(FD));
    ChildrenVisitor::visitFuncDef(FD);
  }

  BasicTypeKind visitNum(Num *) { return BasicTypeKind::Int; }
  BasicTypeKind visitChar(Char *) { return BasicTypeKind::Character; }
  void visitStmt(Stmt *S) { return VisitorBase::visitStmt<void>(S); }

  void setLocalTable(SymbolTableView L) { TheLocalTable = L; }
  void setTable(SymbolTable *S) { TheTable = S; }
  void setFuncDef(FuncDef *FD) { TheFuncDef = FD; }

public:
  TypeCheker() = default;

  // public interface
  bool Check(Program *P, SymbolTable &S) {
    setTable(&S);
    visitProgram(P);
    return EM.IsOk();
  }

private:
  friend class ChildrenVisitor<TypeCheker>;
  friend class VisitorBase<TypeCheker>;

  SymbolTable *TheTable;
  SymbolTableView TheLocalTable;
  FuncDef *TheFuncDef;
  ErrorManager EM;
};

bool CheckType(Program *P, SymbolTable &S) { return TypeCheker().Check(P, S); }

} // namespace simplecompiler
