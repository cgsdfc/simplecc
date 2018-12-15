#include "ByteCodePrinter.h"
#include "Print.h"
#include "Visitor.h"

#include <cassert>
#include <iostream>

namespace simplecompiler {
/// This class handles the formatting of the result of an epxression
/// It is a union of factor Expr and temporary
class ExprValue {
  Expr *Factor;
  int Temporary;

  /// Sanity check
  bool Check() const {
    if (!Factor) {
      return Temporary >= 0;
    } else {
      return (IsInstance<Char>(Factor) || IsInstance<Num>(Factor) ||
              IsInstance<Name>(Factor) || IsInstance<Str>(Factor));
    }
  }

public:
  ExprValue() = default;
  ~ExprValue() = default;
  ExprValue(const ExprValue &) = default;
  ExprValue &operator=(const ExprValue &) = default;

  explicit ExprValue(int Temp) : Factor(nullptr), Temporary(Temp) {
    assert(Check());
  }
  explicit ExprValue(Expr *Factor) : Factor(Factor), Temporary(-1) {
    assert(Check());
  }

  void Format(std::ostream &O) const {
    if (!Factor) {
      O << "t" << Temporary;
    } else if (auto C = subclass_cast<Char>(Factor)) {
      O << "'" << static_cast<char>(C->getC()) << "'";
    } else if (auto N = subclass_cast<Num>(Factor)) {
      O << N->getN();
    } else if (auto N = subclass_cast<Name>(Factor)) {
      O << N->getId();
    } else {
      O << static_cast<Str *>(Factor)->getS();
    }
  }
};

inline std::ostream &operator<<(std::ostream &O, const ExprValue &Val) {
  Val.Format(O);
  return O;
}

/// This class handles the formatting of a label of two form:
/// inline form like GOTO Label_1 and non-inlined form like:
/// Label_1:
/// printf t0
class LineLabel {
  /// No of this label
  unsigned No;
  /// Which form it takes
  bool IsInlined;

public:
  LineLabel(unsigned No) : No(No), IsInlined(false) {}
  LineLabel(const LineLabel &) = default;
  LineLabel &operator=(const LineLabel &) = default;

  /// Setter of inlined, for use in operator<<()
  LineLabel &Inline(bool inlined) {
    this->IsInlined = inlined;
    return *this;
  }

  void Format(std::ostream &O) const {
    O << "Label_" << No;
    if (!IsInlined)
      O << ":";
  }
};

inline std::ostream &operator<<(std::ostream &O, const LineLabel &L) {
  L.Format(O);
  return O;
}

/// This class prints a program in the quarternary form.
class ByteCodePrinter : ChildrenVisitor<ByteCodePrinter> {

  void visitConstDecl(ConstDecl *CD) {
    auto T = CStringFromBasicTypeKind(CD->getType());
    ExprValue val = visitExpr(CD->getValue());
    w.WriteLine("const", T, CD->getName(), "=", val);
  }

  void visitVarDecl(VarDecl *VD) {
    auto T = CStringFromBasicTypeKind(VD->getType());
    if (!VD->getIsArray()) {
      w.WriteLine("var", T, VD->getName());
      return;
    }
    w.getOuts() << "var " << T << " " << VD->getName() << "[" << VD->getSize() << "]\n";
  }

  void visitArgDecl(ArgDecl *AD) {
    w.WriteLine("para", CStringFromBasicTypeKind(AD->getType()), AD->getName());
  }

  void visitFuncDef(FuncDef *FD) {
    w.WriteLine();
    const char *Ret = CStringFromBasicTypeKind(FD->getReturnType());
    w.getOuts() << Ret << " " << FD->getName() << "()\n";
    ChildrenVisitor::visitFuncDef(FD);
    w.WriteLine();
  }

  void visitRead(Read *RD) {
    for (auto name : RD->getNames()) {
      w.WriteLine("scanf", visitExpr(name));
    }
  }

  void visitWrite(Write *WR) {
    if (WR->getStr()) {
      w.WriteLine("printf", visitExpr(WR->getStr()));
    }
    if (WR->getValue()) {
      w.WriteLine("printf", visitExpr(WR->getValue()));
    }
  }

  void visitAssign(Assign *A) {
    ExprValue RHS = visitExpr(A->getValue());
    if (IsInstance<Name>(A->getTarget())) {
      ExprValue LHS = visitExpr(A->getTarget());
      w.WriteLine(LHS, "=", RHS);
      return;
    }

    auto SB = subclass_cast<Subscript>(A->getTarget());
    assert(SB);
    ExprValue Idx = visitExpr(SB->getIndex());
    w.getOuts() << SB->getName() << "[" << Idx << "] = " << RHS << "\n";
  }

  void visitReturn(Return *R) {
    if (R->getValue()) {
      w.WriteLine("ret", visitExpr(R->getValue()));
      return;
    }
    w.WriteLine("ret");
  }

  ExprValue visitCall(Call *C) {
    for (auto A : C->getArgs()) {
      w.WriteLine("push", visitExpr(A));
    }
    w.WriteLine("call", C->getFunc());
    /// return a value even if void function
    ExprValue Ret = MakeTemporary();
    w.WriteLine(Ret, "= RET");
    return Ret;
  }

  LineLabel CompileBoolOp(BoolOp *B) {
    auto OldTmp = getTempCounter();
    ExprValue Val = visitExpr(B->getValue());
    if (OldTmp == getTempCounter()) {
      /// TempCounter didn't increase, we need to hold the result
      w.WriteLine(MakeTemporary(), "=", Val);
    }
    LineLabel LB = MakeLineLabel();
    w.WriteLine("BZ", LB.Inline(true));
    return LB;
  }

  void visitFor(For *F) {
    LineLabel Start = MakeLineLabel();
    visitStmt(F->getInitial());
    w.WriteLine("GOTO", Start.Inline(true));

    LineLabel Loop = MakeLineLabel();
    w.WriteLine(Loop.Inline(false));
    visitStmt(F->getStep());

    LineLabel End = CompileBoolOp(static_cast<BoolOp *>(F->getCondition()));
    w.WriteLine(Start.Inline(false));

    for (Stmt *S : F->getBody()) {
      visitStmt(S);
    }

    w.WriteLine("GOTO", Loop.Inline(true));
    w.WriteLine(End.Inline(false));
  }

  void visitIf(If *I) {
    LineLabel Else = CompileBoolOp(static_cast<BoolOp *>(I->getTest()));

    for (auto S : I->getBody()) {
      visitStmt(S);
    }

    if (I->getOrelse().empty()) {
      w.WriteLine(Else.Inline(false));
      return;
    }

    LineLabel End = MakeLineLabel();
    w.WriteLine("GOTO", End.Inline(true));
    w.WriteLine(Else.Inline(false));

    for (auto S : I->getOrelse()) {
      visitStmt(S);
    }
    w.WriteLine(End.Inline(false));
  }

  void visitWhile(While *W) {
    LineLabel Loop = MakeLineLabel();
    w.WriteLine(Loop.Inline(false));
    LineLabel End = CompileBoolOp(static_cast<BoolOp *>(W->getCondition()));

    for (auto S : W->getBody()) {
      visitStmt(S);
    }

    w.WriteLine("GOTO", Loop.Inline(true));
    w.WriteLine(End.Inline(false));
  }

  ExprValue visitBinOp(BinOp *B) {
    auto Op = CStringFromOperatorKind(B->getOp());
    ExprValue L = visitExpr(B->getLeft());
    ExprValue R = visitExpr(B->getRight());
    ExprValue Result = MakeTemporary();
    w.WriteLine(Result, "=", L, Op, R);
    return Result;
  }

  ExprValue visitUnaryOp(UnaryOp *U) {
    auto Op = CStringFromUnaryopKind(U->getOp());
    ExprValue Operand = visitExpr(U->getOperand());
    ExprValue Result = MakeTemporary();
    w.WriteLine(Result, "=", Op, Operand);
    return Result;
  }

  ExprValue visitSubscript(Subscript *SB) {
    assert(SB->getCtx() == ExprContextKind::Load &&
           "Store must be handle by visitAssign()");
    ExprValue Idx = visitExpr(SB->getIndex());
    ExprValue Result = MakeTemporary();
    w.getOuts() << Result << " = " << SB->getName() << "[" << Idx << "]\n";
    return Result;
  }

  ExprValue visitExpr(Expr *E) { return VisitorBase::visitExpr<ExprValue>(E); }
  ExprValue visitParenExpr(ParenExpr *PE) { return visitExpr(PE->getValue()); }
  ExprValue visitName(Name *N) { return ExprValue(N); }
  ExprValue visitChar(Char *C) { return ExprValue(C); }
  ExprValue visitNum(Num *N) { return ExprValue(N); }
  ExprValue visitStr(Str *S) { return ExprValue(S); }

  ExprValue visitBoolOp(BoolOp *) {
    assert(false && "BoolOp should be handled by CompileBoolOp()");
    return ExprValue();
  }

  ExprValue MakeTemporary() { return ExprValue(TempCounter++); }
  LineLabel MakeLineLabel() { return LineLabel(LabelCounter++); }
  unsigned getTempCounter() const { return TempCounter; }

public:
  ByteCodePrinter(std::ostream &os) : w(os) {}
  ~ByteCodePrinter() = default;

  void Print(Program *P) { visitProgram(P); }

private:
  friend class ChildrenVisitor<ByteCodePrinter>;
  friend class VisitorBase<ByteCodePrinter>;

  Printer w;
  unsigned TempCounter = 0;
  unsigned LabelCounter = 0;
};

void PrintByteCode(Program *P, std::ostream &O) { ByteCodePrinter(O).Print(P); }

} // namespace simplecompiler
