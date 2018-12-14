#include "TypeChecker.h"
#include "ErrorManager.h"
#include "SymbolTable.h"
#include "Visitor.h"

#include <algorithm>

namespace simplecompiler {
// Check type for a function
class TypeCheker : ChildrenVisitor<TypeCheker> {
  // use visitFor, visitWhile of ChildrenVisitor
  friend class ChildrenVisitor<TypeCheker>;
  friend class VisitorBase<TypeCheker>;

  // set expression type of symbolTable
  SymbolTable &TheTable;
  // local symbol information (type information mainly)
  SymbolTableView local;
  // FuncDef node being checked
  FuncDef *funcDef;
  // report errors to ErrorManager
  ErrorManager &e;

  // return type of the function being checked
  BasicTypeKind GetReturnType() const { return funcDef->getReturnType(); }

  // name of the function being checked
  const String &GetFuncName() const { return funcDef->getName(); }

  void visitStmt(Stmt *s) { return VisitorBase::visitStmt<void>(s); }

  // Return the type of evaluating the expression
  BasicTypeKind visitExpr(Expr *node) {
    auto type = VisitorBase::visitExpr<BasicTypeKind>(node);
    TheTable.setExprType(node, type);
    return type;
  }

  void visitRead(Read *RD) {
    for (auto expr : RD->getNames()) {
      visitExpr(expr); // Collect type info.
      auto name = subclass_cast<Name>(expr);
      const auto &entry = local[name->getId()];
      if (!entry.IsVariable()) {
        e.TypeError(name->getLoc(), "cannot use scanf() on object of type",
                    entry.GetTypeName());
      }
    }
  }

  void visitWrite(Write *WR) {
    if (WR->getValue()) {
      CheckExprOperand(WR->getValue());
    }
  }

  void visitReturn(Return *node) {
    auto return_type =
        node->getValue() ? visitExpr(node->getValue()) : BasicTypeKind::Void;

    // order a strict match
    if (GetReturnType() != return_type) {
      e.TypeError(node->getLoc(), "return type mismatched:", "function",
                  Quote(GetFuncName()), "must return",
                  CStringFromBasicTypeKind(GetReturnType()), "not",
                  CStringFromBasicTypeKind(return_type));
    }
  }

  void visitAssign(Assign *node) {
    int errs = e.GetErrorCount();
    auto value = CheckExprOperand(node->getValue());
    auto target = visitExpr(node->getTarget());

    if (e.IsOk(errs) && target != value) {
      e.TypeError(node->getLoc(), "type mismatched in assignment:",
                  CStringFromBasicTypeKind(target), "=",
                  CStringFromBasicTypeKind(value));
    }
  }

  void visitExprStmt(ExprStmt *node) {
    auto call = subclass_cast<Call>(node->getValue());
    assert(call && "value of ExprStmt must be a Call");
    visitCall(call);
  }

  // check the operand of BoolOp, restrict to int
  void CheckBoolOpOperand(Expr *operand) {
    auto msg = "operands of condition must be of type int";
    auto errs = e.GetErrorCount();
    auto type = visitExpr(operand);
    if (e.IsOk(errs) && type != BasicTypeKind::Int) {
      e.TypeError(operand->getLoc(), msg);
    }
  }

  BasicTypeKind visitBoolOp(BoolOp *node) {
    if (node->getHasCmpop()) {
      auto x = static_cast<BinOp *>(node->getValue());
      CheckBoolOpOperand(x->getLeft());
      CheckBoolOpOperand(x->getRight());
    } else {
      CheckBoolOpOperand(node->getValue());
    }
    return BasicTypeKind::Int;
  }

  // check the operand of Expr, restrict to NOT void
  BasicTypeKind CheckExprOperand(Expr *expr) {
    auto msg = "void value cannot be used in an expression";
    auto errs = e.GetErrorCount();
    auto type = visitExpr(expr);
    if (e.IsOk(errs) && type == BasicTypeKind::Void) {
      e.TypeError(expr->getLoc(), msg);
    }
    return type;
  }

  BasicTypeKind visitBinOp(BinOp *node) {
    CheckExprOperand(node->getLeft());
    CheckExprOperand(node->getRight());
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitUnaryOp(UnaryOp *node) {
    CheckExprOperand(node->getOperand());
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitParenExpr(ParenExpr *node) {
    CheckExprOperand(node->getValue());
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitCall(Call *node) {
    const auto &entry = local[node->getFunc()];
    if (!entry.IsFunction()) {
      e.TypeError(node->getLoc(), "object of type", entry.GetTypeName(),
                  "cannot be called as a function");
      return BasicTypeKind::Void;
    }

    auto fun_type = entry.AsFunction();
    auto formal_args_len = fun_type.GetArgCount();
    auto actual_args_len = node->args.size();
    if (formal_args_len != actual_args_len) {
      e.TypeError(node->getLoc(), "function", Quote(node->getFunc()), "expects",
                  formal_args_len, "arguments, got", actual_args_len);
    }

    // check args
    auto len = std::min(formal_args_len, actual_args_len);
    for (int i = 0; i < len; i++) {
      auto actual = visitExpr(node->getArgs()[i]);
      auto formal = fun_type.GetArgTypeAt(i);
      if (actual != formal) {
        e.TypeError(node->getArgs()[i]->getLoc(), "argument", i + 1,
                    "of function", Quote(node->getFunc()), "must be",
                    CStringFromBasicTypeKind(formal), ", not",
                    CStringFromBasicTypeKind(actual));
      }
    }
    return fun_type.GetReturnType();
  }

  BasicTypeKind visitSubscript(Subscript *node) {
    const auto &entry = local[node->getName()];
    if (!entry.IsArray()) {
      e.TypeError(node->getLoc(), "object of type", entry.GetTypeName(),
                  "cannot be subscripted as an array");
      return BasicTypeKind::Void;
    }

    int errs = e.GetErrorCount();
    auto index = visitExpr(node->getIndex());
    if (e.IsOk(errs) && index != BasicTypeKind::Int) {
      e.TypeError(node->getLoc(), "type of array index must be int");
    }
    return entry.AsArray().GetElementType();
  }

  BasicTypeKind visitName(Name *node) {
    const auto &entry = local[node->getId()];
    if (node->getCtx() == ExprContextKind::Load && entry.IsArray()) {
      e.TypeError(node->getLoc(), "object of type", entry.GetTypeName(),
                  "cannot be used in an expression");
      return BasicTypeKind::Void;
    }
    if (node->getCtx() == ExprContextKind::Store && !entry.IsVariable()) {
      e.TypeError(node->getLoc(), "object of type", entry.GetTypeName(),
                  "cannot be assigned to");
      return BasicTypeKind::Void;
    }
    if (entry.IsConstant())
      return entry.AsConstant().GetType();
    return entry.AsVariable().GetType();
  }

  BasicTypeKind visitNum(Num *x) { return BasicTypeKind::Int; }
  // not actually used, for instantiation only
  BasicTypeKind visitStr(Str *x) {
    e.InternalError(x->getLoc(), "TypeCheker::visitStr() shall not be called");
  }
  BasicTypeKind visitChar(Char *x) { return BasicTypeKind::Character; }

public:
  TypeCheker(SymbolTable &TheTable, FuncDef *fun, ErrorManager &e)
      : TheTable(TheTable), local(TheTable.getLocalTable(fun)), funcDef(fun),
        e(e) {}

  // public interface
  void Check() {
    for (auto stmt : funcDef->getStmts()) {
      visitStmt(stmt);
    }
  }
};
} // namespace simplecompiler

bool simplecompiler::CheckType(Program *prog, SymbolTable &symtable) {
  ErrorManager e;
  for (auto decl : prog->getDecls()) {
    if (auto fun = subclass_cast<FuncDef>(decl)) {
      TypeCheker(symtable, fun, e).Check();
    }
  }
  return e.IsOk();
}
