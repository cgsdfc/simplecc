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
    auto type = CStringFromBasicTypeKind(CD->getType());
    ExprValue val = visitExpr(CD->getValue());
    w.WriteLine("const", type, CD->getName(), "=", val);
  }

  void visitVarDecl(VarDecl *VD) {
    auto type = CStringFromBasicTypeKind(VD->getType());
    if (!VD->getIsArray()) {
      w.WriteLine("var", type, VD->getName());
    } else {
      w.WriteLine("var", type, VD->getName(), "[", VD->getSize(), "]");
    }
  }

  void visitArgDecl(ArgDecl *AD) {
    w.WriteLine("para", CStringFromBasicTypeKind(AD->getType()), AD->getName());
  }

  void visitFuncDef(FuncDef *FD) {
    w.WriteLine(CStringFromBasicTypeKind(FD->getReturnType()), FD->getName(),
                "()");
    ChildrenVisitor::visitFuncDef(FD);
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
    ExprValue value = visitExpr(A->getValue());
    if (IsInstance<Name>(A->getTarget())) {
      ExprValue target = visitExpr(A->getTarget());
      w.WriteLine(target, "=", value);
      return;
    }

    auto subscr = subclass_cast<Subscript>(A->getTarget());
    assert(subscr);
    ExprValue index = visitExpr(subscr->getIndex());
    w.WriteLine(subscr->getName(), "[", index, "] =", value);
  }

  void visitReturn(Return *R) {
    if (R->getValue()) {
      w.WriteLine("ret", visitExpr(R->getValue()));
      return;
    }
    w.WriteLine("ret");
  }

  ExprValue visitCall(Call *C) {
    for (auto arg : C->getArgs()) {
      w.WriteLine("push", visitExpr(arg));
    }
    w.WriteLine("call", C->getFunc());
    /// return a value even if void function
    ExprValue ret_val = MakeTemporary();
    w.WriteLine(ret_val, "= RET");
    return ret_val;
  }

  LineLabel CompileBoolOp(BoolOp *B) {
    auto temps = TempCounter;
    ExprValue val = visitExpr(B->getValue());
    if (temps == TempCounter) {
      /// TempCounter didn't increase, we need to hold the result
      w.WriteLine(MakeTemporary(), "=", val);
    }
    LineLabel label = MakeLineLabel();
    w.WriteLine("BZ", label.Inline(true));
    return label;
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

    for (auto s : I->getBody()) {
      visitStmt(s);
    }

    if (I->getOrelse().empty()) {
      w.WriteLine(Else.Inline(false));
      return;
    }

    LineLabel End = MakeLineLabel();
    w.WriteLine("GOTO", End.Inline(true));
    w.WriteLine(Else.Inline(false));

    for (auto s : I->getOrelse()) {
      visitStmt(s);
    }
    w.WriteLine(End.Inline(false));
  }

  void visitWhile(While *W) {
    LineLabel Loop = MakeLineLabel();
    w.WriteLine(Loop.Inline(false));
    LineLabel End = CompileBoolOp(static_cast<BoolOp *>(W->getCondition()));

    for (auto s : W->getBody()) {
      visitStmt(s);
    }

    w.WriteLine("GOTO", Loop.Inline(true));
    w.WriteLine(End.Inline(false));
  }

  ExprValue visitBinOp(BinOp *B) {
    auto op = CStringFromOperatorKind(B->getOp());
    ExprValue L = visitExpr(B->getLeft());
    ExprValue R = visitExpr(B->getRight());
    ExprValue Result = MakeTemporary();
    w.WriteLine(Result, "=", L, op, R);
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
    w.WriteLine(Result, "=", SB->getName(), "[", Idx, "]");
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

  void visitDecl(Decl *D) {
    /// Line break before each function.
    if (IsInstance<FuncDef>(D))
      w.WriteLine();
    ChildrenVisitor::visitDecl(D);
  }

public:
  ByteCodePrinter(std::ostream &os) : w(os) {}

  void Print(Program *P) { visitProgram(P); }

private:
  friend class ChildrenVisitor<ByteCodePrinter>;
  friend class VisitorBase<ByteCodePrinter>;

  Printer w;
  int TempCounter = 0;
  int LabelCounter = 0;
};

void PrintByteCode(Program *P, std::ostream &O) { ByteCodePrinter(O).Print(P); }

} // namespace simplecompiler
