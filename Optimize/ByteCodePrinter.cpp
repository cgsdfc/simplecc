#include "ByteCodePrinter.h"
#include "Print.h"
#include "Visitor.h"

#include <cassert>
#include <iostream>

using namespace simplecompiler;

namespace {
// A class that handles the formatting of the result of an epxression
// It is a union of factor expr and temporary
class ExprValue {
  Expr *factor;
  int temporary;

  // Sanity check
  bool Check() const {
    if (!factor) {
      return temporary >= 0;
    } else {
      return (IsInstance<Char>(factor) || IsInstance<Num>(factor) ||
              IsInstance<Name>(factor) || IsInstance<Str>(factor));
    }
  }

public:
  ExprValue() = default;

  explicit ExprValue(int temporary) : factor(nullptr), temporary(temporary) {
    assert(Check());
  }
  explicit ExprValue(Expr *factor) : factor(factor), temporary(-1) {
    assert(Check());
  }

  void Format(std::ostream &os) const {
    if (!factor) {
      os << "t" << temporary;
    } else if (auto x = subclass_cast<Char>(factor)) {
      os << "'" << static_cast<char>(x->getC()) << "'";
    } else if (auto x = subclass_cast<Num>(factor)) {
      os << x->getN();
    } else if (auto x = subclass_cast<Name>(factor)) {
      os << x->getId();
    } else {
      os << static_cast<Str *>(factor)->getS();
    }
  }
};

std::ostream &operator<<(std::ostream &os, const ExprValue &c) {
  c.Format(os);
  return os;
}

// A class that handles the formatting of a label of two form:
// inline form like GOTO Label_1 and non-inlined form like:
// Label_1:
// printf t0
class LineLabel {
  // Identifier of this label
  int value;
  // which form it takes
  bool inlined;

public:
  LineLabel(int value) : value(value), inlined(false) {}

  // setter of inlined, for use in operator<<()
  LineLabel &Inline(bool inlined) {
    this->inlined = inlined;
    return *this;
  }

  void Format(std::ostream &os) const {
    os << "Label_" << value;
    if (!inlined)
      os << ":";
  }
};

std::ostream &operator<<(std::ostream &os, const LineLabel &c) {
  c.Format(os);
  return os;
}

// A class that prints a program in the quarternary form
class ByteCodePrinter : VisitorBase<ByteCodePrinter> {
  friend class VisitorBase<ByteCodePrinter>;

  Program *program;
  Printer w;
  // temporaries counter
  int temporaries;
  // label counter
  int labels;

  ExprValue MakeTemporary() { return ExprValue(temporaries++); }

  LineLabel MakeLineLabel() { return LineLabel(labels++); }

  void visitStmt(Stmt *s) { return VisitorBase::visitStmt<void>(s); }

  ExprValue visitExpr(Expr *s) { return VisitorBase::visitExpr<ExprValue>(s); }

  void visitDecl(Decl *s) { return VisitorBase::visitDecl<void>(s); }

  void visitConstDecl(ConstDecl *node) {
    auto type = CStringFromBasicTypeKind(node->getType());
    auto &&val = visitExpr(node->getValue());
    w.WriteLine("const", type, node->getName(), "=", val);
  }

  void visitVarDecl(VarDecl *node) {
    auto type = CStringFromBasicTypeKind(node->getType());
    if (!node->getIsArray()) {
      w.WriteLine("var", type, node->getName());
    } else {
      w.WriteLine("var", type, node->getName(), "[", node->getSize(), "]");
    }
  }

  void visitArgDecl(ArgDecl *AD) {
    w.WriteLine("para", CStringFromBasicTypeKind(AD->getType()), AD->getName());
  }

  void visitFuncDef(FuncDef *node) {
    w.WriteLine(CStringFromBasicTypeKind(node->getReturnType()), node->getName(), "()");
    for (auto arg : node->getArgs()) {
      visitDecl(arg);
    }
    for (auto decl : node->getDecls()) {
      visitDecl(decl);
    }
    for (auto stmt : node->getStmts()) {
      visitStmt(stmt);
    }
    // XXX: No return here!
  }

  void visitRead(Read *node) {
    for (auto name : node->getNames()) {
      w.WriteLine("scanf", visitExpr(name));
    }
  }

  void visitWrite(Write *node) {
    if (node->getStr()) {
      w.WriteLine("printf", visitExpr(node->getStr()));
    }
    if (node->getValue()) {
      w.WriteLine("printf", visitExpr(node->getValue()));
    }
  }

  void visitAssign(Assign *node) {
    auto &&value = visitExpr(node->getValue());
    if (IsInstance<Name>(node->getTarget())) {
      auto &&target = visitExpr(node->getTarget());
      w.WriteLine(target, "=", value);
    } else {
      auto subscr = subclass_cast<Subscript>(node->getTarget());
      assert(subscr);
      auto &&index = visitExpr(subscr->getIndex());
      w.WriteLine(subscr->getName(), "[", index, "] =", value);
    }
  }

  void visitReturn(Return *node) {
    if (node->getValue()) {
      w.WriteLine("ret", visitExpr(node->getValue()));
    } else {
      w.WriteLine("ret");
    }
  }

  ExprValue visitCall(Call *node) {
    for (auto arg : node->getArgs()) {
      w.WriteLine("push", visitExpr(arg));
    }
    w.WriteLine("call", node->getFunc());
    // return a value even if void function
    auto &&ret_val = MakeTemporary();
    w.WriteLine(ret_val, "= RET");
    return ret_val;
  }

  LineLabel CompileBoolOp(BoolOp *node, bool jump_if_false) {
    auto temps = temporaries;
    auto &&val = visitExpr(node->getValue());
    if (temps == temporaries) {
      // temporaries didn't increase, we need to hold the result
      w.WriteLine(MakeTemporary(), "=", val);
    }
    auto &&label = MakeLineLabel();
    auto jump = jump_if_false ? "BZ" : "BNZ";
    w.WriteLine(jump, label.Inline(true));
    return label;
  }

  void visitFor(For *node) {
    auto &&start_label = MakeLineLabel();
    visitStmt(node->getInitial());
    w.WriteLine("GOTO", start_label.Inline(true));
    auto &&loop_label = MakeLineLabel();
    w.WriteLine(loop_label.Inline(false));
    visitStmt(node->getStep());
    auto &&end_label =
        CompileBoolOp(static_cast<BoolOp *>(node->getCondition()), true);
    w.WriteLine(start_label.Inline(false));
    for (auto s : node->getBody()) {
      visitStmt(s);
    }
    w.WriteLine("GOTO", loop_label.Inline(true));
    w.WriteLine(end_label.Inline(false));
  }

  void visitIf(If *node) {
    auto &&orelse_label =
        CompileBoolOp(static_cast<BoolOp *>(node->getTest()), true);
    for (auto s : node->getBody()) {
      visitStmt(s);
    }
    if (node->getOrelse().empty()) {
      w.WriteLine(orelse_label.Inline(false));
      return;
    }
    auto &&end_label = MakeLineLabel();
    w.WriteLine("GOTO", end_label.Inline(true));
    w.WriteLine(orelse_label.Inline(false));
    for (auto s : node->getOrelse()) {
      visitStmt(s);
    }
    w.WriteLine(end_label.Inline(false));
  }

  void visitWhile(While *node) {
    auto &&loop_label = MakeLineLabel();
    w.WriteLine(loop_label.Inline(false));
    auto &&end_label =
        CompileBoolOp(static_cast<BoolOp *>(node->getCondition()), true);
    for (auto s : node->getBody()) {
      visitStmt(s);
    }
    w.WriteLine("GOTO", loop_label.Inline(true));
    w.WriteLine(end_label.Inline(false));
  }

  void visitExprStmt(ExprStmt *node) { visitExpr(node->getValue()); }

  ExprValue visitName(Name *node) { return ExprValue(node); }

  ExprValue visitChar(Char *node) { return ExprValue(node); }

  ExprValue visitNum(Num *node) { return ExprValue(node); }

  ExprValue visitStr(Str *node) { return ExprValue(node); }

  ExprValue visitParenExpr(ParenExpr *node) { return visitExpr(node->getValue()); }

  ExprValue visitBoolOp(BoolOp *node) {
    assert(false && "BoolOp should be handled by CompileBoolOp()");
    return ExprValue();
  }

  // XXX: result id should come after operands's
  ExprValue visitBinOp(BinOp *node) {
    auto op = CStringFromOperatorKind(node->getOp());
    auto &&left = visitExpr(node->getLeft());
    auto &&right = visitExpr(node->getRight());
    auto &&result = MakeTemporary();
    w.WriteLine(result, "=", left, op, right);
    return result;
  }

  ExprValue visitUnaryOp(UnaryOp *node) {
    auto op = CStringFromUnaryopKind(node->getOp());
    auto &&operand = visitExpr(node->getOperand());
    auto &&result = MakeTemporary();
    w.WriteLine(result, "=", op, operand);
    return result;
  }

  ExprValue visitSubscript(Subscript *node) {
    assert(node->getCtx() == ExprContextKind::Load &&
           "Store must be handle by visitAssign()");
    auto &&index = visitExpr(node->getIndex());
    auto &&result = MakeTemporary();
    w.WriteLine(result, "=", node->getName(), "[", index, "]");
    return result;
  }

public:
  ByteCodePrinter(Program *program, std::ostream &os)
      : program(program), w(os), temporaries(0), labels(0) {}

  void Print() {
    for (auto decl : program->getDecls()) {
      visitDecl(decl);
      w.WriteLine();
    }
  }
};
} // namespace

void simplecompiler::PrintByteCode(Program *prog, std::ostream &os) {
  ByteCodePrinter(prog, os).Print();
}
