#include "simplecompiler/TypeChecker.h"
#include "simplecompiler/ErrorManager.h"
#include "simplecompiler/SymbolTable.h"
#include "simplecompiler/Visitor.h"

#include <algorithm>

namespace simplecompiler {
// Check type for a function
class TypeCheker : ChildrenVisitor<TypeCheker> {

  void visitRead(Read *RD) {
    for (auto E : RD->getNames()) {
      auto N = subclass_cast<Name>(E);
      assert(N);
      const auto &Entry = TheLocalTable[N->getId()];
      if (!Entry.IsVariable()) {
        EM.TypeError(N->getLoc(), "cannot use scanf() on", Entry.GetTypeName(),
                     Entry.GetName());
        continue;
      }
      /// set the Expr type for this.
      TheTable->setExprType(E, Entry.AsVariable().GetType());
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
    int Errs = EM.GetErrorCount();
    auto RHS = CheckExprOperand(A->getValue());
    auto LHS = visitExpr(A->getTarget());

    if (EM.IsOk(Errs) && LHS != RHS) {
      EM.TypeError(A->getLoc(), "type mismatched in assignment:",
                   CStringFromBasicTypeKind(LHS), "=",
                   CStringFromBasicTypeKind(RHS));
    }
  }

  // check the operand of BoolOp, restrict to int
  void CheckBoolOpOperand(Expr *E) {
    auto Msg = "operands of condition must be of type int";
    auto Errs = EM.GetErrorCount();
    auto T = visitExpr(E);
    if (EM.IsOk(Errs) && T != BasicTypeKind::Int) {
      EM.TypeError(E->getLoc(), Msg);
    }
  }

  BasicTypeKind visitBoolOp(BoolOp *B) {
    if (B->getHasCmpop()) {
      auto Bin = static_cast<BinOp *>(B->getValue());
      CheckBoolOpOperand(Bin->getLeft());
      CheckBoolOpOperand(Bin->getRight());
    } else {
      CheckBoolOpOperand(B->getValue());
    }
    return BasicTypeKind::Int;
  }

  // check the operand of Expr, restrict to NOT void
  BasicTypeKind CheckExprOperand(Expr *E) {
    auto Msg = "void value cannot be used in an expression";
    auto Errs = EM.GetErrorCount();
    auto T = visitExpr(E);
    if (EM.IsOk(Errs) && T == BasicTypeKind::Void) {
      EM.TypeError(E->getLoc(), Msg);
    }
    return T;
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
    const auto &Entry = TheLocalTable[C->getFunc()];
    if (!Entry.IsFunction()) {
      EM.TypeError(C->getLoc(), Entry.GetTypeName(), Entry.GetName(),
                   "cannot be called as a function");
      return BasicTypeKind::Void;
    }

    auto Ty = Entry.AsFunction();
    auto NumFormal = Ty.GetArgCount();
    auto NumActual = C->getArgs().size();
    if (NumFormal != NumActual) {
      EM.TypeError(C->getLoc(), "function", Quote(C->getFunc()), "expects",
                   NumFormal, "arguments, got", NumActual);
    }

    // check args
    auto Size = std::min(NumFormal, NumActual);
    for (int I = 0; I < Size; I++) {
      auto ActualTy = visitExpr(C->getArgs()[I]);
      auto FormalTy = Ty.GetArgTypeAt(I);
      if (ActualTy != FormalTy) {
        EM.TypeError(C->getArgs()[I]->getLoc(), "argument", I + 1,
                     "of function", Quote(C->getFunc()), "must be",
                     CStringFromBasicTypeKind(FormalTy), ", not",
                     CStringFromBasicTypeKind(ActualTy));
      }
    }
    return Ty.GetReturnType();
  }

  BasicTypeKind visitSubscript(Subscript *SB) {
    const auto &Entry = TheLocalTable[SB->getName()];
    if (!Entry.IsArray()) {
      EM.TypeError(SB->getLoc(), Entry.GetTypeName(), Entry.GetName(),
                   "cannot be subscripted as an array");
      return BasicTypeKind::Void;
    }

    int Errs = EM.GetErrorCount();
    auto Idx = visitExpr(SB->getIndex());
    if (EM.IsOk(Errs) && Idx != BasicTypeKind::Int) {
      EM.TypeError(SB->getLoc(), "Array index must be int");
    }
    return Entry.AsArray().GetElementType();
  }

  BasicTypeKind visitName(Name *N) {
    const auto &Entry = TheLocalTable[N->getId()];
    if (N->getCtx() == ExprContextKind::Load && Entry.IsArray()) {
      EM.TypeError(N->getLoc(), Entry.GetTypeName(), Entry.GetName(),
                   "cannot be used in an expression");
      return BasicTypeKind::Void;
    }
    if (N->getCtx() == ExprContextKind::Store && !Entry.IsVariable()) {
      EM.TypeError(N->getLoc(), Entry.GetTypeName(), Entry.GetName(),
                   "cannot be assigned to");
      return BasicTypeKind::Void;
    }
    if (Entry.IsConstant())
      return Entry.AsConstant().GetType();
    return Entry.AsVariable().GetType();
  }

  // not actually used, for instantiation only
  BasicTypeKind visitStr(Str *) {
    assert(false && "TypeCheker::visitStr() shall not be called");
    return BasicTypeKind::Void;
  }

  // Return the type of evaluating the expression
  BasicTypeKind visitExpr(Expr *E) {
    auto T = VisitorBase::visitExpr<BasicTypeKind>(E);
    TheTable->setExprType(E, T);
    return T;
  }

  void visitFuncDef(FuncDef *FD) {
    assert(TheTable);
    setFuncDef(FD);
    setLocalTable(TheTable->getLocalTable(FD));
    ChildrenVisitor::visitFuncDef(FD);
  }

  BasicTypeKind visitNum(Num *) { return BasicTypeKind::Int; }
  BasicTypeKind visitChar(Char *) { return BasicTypeKind::Character; }

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
