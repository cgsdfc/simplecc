#include "ByteCodePrinter.h"
#include "Print.h"
#include "Visitor.h"

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
      os << "'" << static_cast<char>(x->c) << "'";
    } else if (auto x = subclass_cast<Num>(factor)) {
      os << x->n;
    } else if (auto x = subclass_cast<Name>(factor)) {
      os << x->id;
    } else {
      os << static_cast<Str *>(factor)->s;
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
    auto type = CStringFromBasicTypeKind(node->type);
    auto &&val = visitExpr(node->value);
    w.WriteLine("const", type, node->name, "=", val);
  }

  void visitVarDecl(VarDecl *node) {
    auto type = CStringFromBasicTypeKind(node->type);
    if (!node->is_array) {
      w.WriteLine("var", type, node->name);
    } else {
      w.WriteLine("var", type, node->name, "[", node->size, "]");
    }
  }

  void visitArgDecl(ArgDecl *AD) {
      w.WriteLine("para", CStringFromBasicTypeKind(AD->type), AD->name);
  }

  void visitFuncDef(FuncDef *node) {
    w.WriteLine(CStringFromBasicTypeKind(node->return_type), node->name, "()");
    for (auto arg : node->args) {
      visitDecl(arg);
    }
    for (auto decl : node->decls) {
      visitDecl(decl);
    }
    for (auto stmt : node->stmts) {
      visitStmt(stmt);
    }
    // XXX: No return here!
  }

  void visitRead(Read *node) {
    for (auto name : node->names) {
      w.WriteLine("scanf", visitExpr(name));
    }
  }

  void visitWrite(Write *node) {
    if (node->str) {
      w.WriteLine("printf", visitExpr(node->str));
    }
    if (node->value) {
      w.WriteLine("printf", visitExpr(node->value));
    }
  }

  void visitAssign(Assign *node) {
    auto &&value = visitExpr(node->value);
    if (IsInstance<Name>(node->target)) {
      auto &&target = visitExpr(node->target);
      w.WriteLine(target, "=", value);
    } else {
      auto subscr = subclass_cast<Subscript>(node->target);
      assert(subscr);
      auto &&index = visitExpr(subscr->index);
      w.WriteLine(subscr->name, "[", index, "] =", value);
    }
  }

  void visitReturn(Return *node) {
    if (node->value) {
      w.WriteLine("ret", visitExpr(node->value));
    } else {
      w.WriteLine("ret");
    }
  }

  ExprValue visitCall(Call *node) {
    for (auto arg : node->args) {
      w.WriteLine("push", visitExpr(arg));
    }
    w.WriteLine("call", node->func);
    // return a value even if void function
    auto &&ret_val = MakeTemporary();
    w.WriteLine(ret_val, "= RET");
    return ret_val;
  }

  LineLabel CompileBoolOp(BoolOp *node, bool jump_if_false) {
    auto temps = temporaries;
    auto &&val = visitExpr(node->value);
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
    visitStmt(node->initial);
    w.WriteLine("GOTO", start_label.Inline(true));
    auto &&loop_label = MakeLineLabel();
    w.WriteLine(loop_label.Inline(false));
    visitStmt(node->step);
    auto &&end_label =
        CompileBoolOp(static_cast<BoolOp *>(node->condition), true);
    w.WriteLine(start_label.Inline(false));
    for (auto s : node->body) {
      visitStmt(s);
    }
    w.WriteLine("GOTO", loop_label.Inline(true));
    w.WriteLine(end_label.Inline(false));
  }

  void visitIf(If *node) {
    auto &&orelse_label =
        CompileBoolOp(static_cast<BoolOp *>(node->test), true);
    for (auto s : node->body) {
      visitStmt(s);
    }
    if (node->orelse.empty()) {
      w.WriteLine(orelse_label.Inline(false));
      return;
    }
    auto &&end_label = MakeLineLabel();
    w.WriteLine("GOTO", end_label.Inline(true));
    w.WriteLine(orelse_label.Inline(false));
    for (auto s : node->orelse) {
      visitStmt(s);
    }
    w.WriteLine(end_label.Inline(false));
  }

  void visitWhile(While *node) {
    auto &&loop_label = MakeLineLabel();
    w.WriteLine(loop_label.Inline(false));
    auto &&end_label =
        CompileBoolOp(static_cast<BoolOp *>(node->condition), true);
    for (auto s : node->body) {
      visitStmt(s);
    }
    w.WriteLine("GOTO", loop_label.Inline(true));
    w.WriteLine(end_label.Inline(false));
  }

  void visitExprStmt(ExprStmt *node) { visitExpr(node->value); }

  ExprValue visitName(Name *node) { return ExprValue(node); }

  ExprValue visitChar(Char *node) { return ExprValue(node); }

  ExprValue visitNum(Num *node) { return ExprValue(node); }

  ExprValue visitStr(Str *node) { return ExprValue(node); }

  ExprValue visitParenExpr(ParenExpr *node) { return visitExpr(node->value); }

  ExprValue visitBoolOp(BoolOp *node) {
    assert(false && "BoolOp should be handled by CompileBoolOp()");
  }

  // XXX: result id should come after operands's
  ExprValue visitBinOp(BinOp *node) {
    auto op = CStringFromOperatorKind(node->op);
    auto &&left = visitExpr(node->left);
    auto &&right = visitExpr(node->right);
    auto &&result = MakeTemporary();
    w.WriteLine(result, "=", left, op, right);
    return result;
  }

  ExprValue visitUnaryOp(UnaryOp *node) {
    auto op = CStringFromUnaryopKind(node->op);
    auto &&operand = visitExpr(node->operand);
    auto &&result = MakeTemporary();
    w.WriteLine(result, "=", op, operand);
    return result;
  }

  ExprValue visitSubscript(Subscript *node) {
    assert(node->ctx == ExprContextKind::Load &&
           "Store must be handle by visitAssign()");
    auto &&index = visitExpr(node->index);
    auto &&result = MakeTemporary();
    w.WriteLine(result, "=", node->name, "[", index, "]");
    return result;
  }

public:
  ByteCodePrinter(Program *program, std::ostream &os)
      : program(program), w(os), temporaries(0), labels(0) {}

  void Print() {
    for (auto decl : program->decls) {
      visitDecl(decl);
      w.WriteLine();
    }
  }
};
} // namespace

void simplecompiler::PrintByteCode(Program *prog, std::ostream &os) {
  ByteCodePrinter(prog, os).Print();
}
