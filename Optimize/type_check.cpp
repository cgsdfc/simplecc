#include "type_check.h"
#include "SymbolTable.h"
#include "Visitor.h"
#include "error.h"

namespace {
using namespace simplecompiler;
// Transform Name to Call if it is a function and it is in the context
// of Expr or ExprStmt (Call really).
class ImplicitCallTransformer : public VisitorBase<ImplicitCallTransformer> {
  FuncDef *funcDef;
  SymbolTableView local;

public:
  ImplicitCallTransformer(const SymbolTable &symtable, FuncDef *fun)
      : funcDef(fun), local(symtable.GetLocal(fun)) {}

  void visitStmt(Stmt *node) { VisitorBase::visitStmt<void>(node); }

  Expr *visitExpr(Expr *node) {
    if (auto x = subclass_cast<Name>(node)) {
      if (local[x->id].IsFunction()) {
        // replace such a name with a call
        auto call = new Call(x->id, {}, x->loc);
        delete x;
        return call;
      } else {
        return node;
      }
    } else {
      VisitorBase::visitExpr<void>(node);
      return node;
    }
  }

#define VISIT(name)                                                            \
  do {                                                                         \
    node->name = visitExpr(node->name);                                        \
  } while (0)

  // don't check names in Read
  void visitRead(Read *node) {}

  void visitWrite(Write *node) {
    if (node->value) {
      VISIT(value);
    }
  }

  void visitAssign(Assign *node) {
    // don't check Name
    if (IsInstance<Subscript>(node->target)) {
      visitExpr(node->target);
    }
    VISIT(value);
  }

  void visitFor(For *node) {
    visitStmt(node->initial);
    VISIT(condition);
    visitStmt(node->step);
    for (auto s : node->body) {
      visitStmt(s);
    }
  }

  void visitWhile(While *node) {
    VISIT(condition);
    for (auto s : node->body) {
      visitStmt(s);
    }
  }

  void visitReturn(Return *node) {
    if (node->value) {
      VISIT(value);
    }
  }

  void visitIf(If *node) {
    VISIT(test);
    for (auto s : node->body) {
      visitStmt(s);
    }
    for (auto s : node->orelse) {
      visitStmt(s);
    }
  }

  // nothing to do since it must be a Call
  void visitExprStmt(ExprStmt *node) {
    assert(IsInstance<Call>(node->value));
    visitExpr(node->value);
  }

  void visitBinOp(BinOp *node) {
    VISIT(left);
    VISIT(right);
  }

  void visitBoolOp(BoolOp *node) { VISIT(value); }

  void visitParenExpr(ParenExpr *node) { VISIT(value); }

  void visitUnaryOp(UnaryOp *node) { VISIT(operand); }

  void visitCall(Call *node) {
    for (int i = 0, size = node->args.size(); i < size; i++) {
      node->args[i] = visitExpr(node->args[i]);
    }
  }

  void visitNum(Num *) {}
  void visitStr(Str *) {}
  void visitChar(Char *) {}
  void visitName(Name *) {
    // calling this is an error
    assert(false && "All Names must be handled by container nodes");
  }

  void visitSubscript(Subscript *node) { VISIT(index); }

  // public interface
  void Transform() {
    for (auto stmt : funcDef->stmts) {
      visitStmt(stmt);
    }
  }

#undef VISIT
};

// Check type for a function
class TypeCheker : public ChildrenVisitor<TypeCheker> {
  // use visitFor, visitWhile of ChildrenVisitor

  // set expression type of symbolTable
  SymbolTable &symbolTable;
  // local symbol information (type information mainly)
  SymbolTableView local;
  // FuncDef node being checked
  FuncDef *funcDef;
  // report errors to ErrorManager
  ErrorManager &e;

  // return type of the function being checked
  BasicTypeKind GetReturnType() const { return funcDef->return_type; }

  // name of the function being checked
  const String &GetFuncName() const { return funcDef->name; }

public:
  TypeCheker(SymbolTable &symbolTable, FuncDef *fun, ErrorManager &e)
      : symbolTable(symbolTable), local(symbolTable.GetLocal(fun)),
        funcDef(fun), e(e) {}

  // public interface
  void Check() {
    for (auto stmt : funcDef->stmts) {
      visitStmt(stmt);
    }
  }

  void visitStmt(Stmt *s) { return VisitorBase::visitStmt<void>(s); }

  // Return the type of evaluating the expression
  BasicTypeKind visitExpr(Expr *node) {
    auto type = VisitorBase::visitExpr<BasicTypeKind>(node);
    symbolTable.SetExprType(node, type);
    return type;
  }

  void visitRead(Read *node) {
    for (auto expr : node->names) {
      auto name = subclass_cast<Name>(expr);
      const auto &entry = local[name->id];
      if (!entry.IsVariable()) {
        e.TypeError(name->loc, "cannot use scanf() on object of type",
                    entry.GetTypeName());
      }
    }
  }

  void visitWrite(Write *node) {
    if (node->value) {
      CheckExprOperand(node->value);
    }
  }

  void visitReturn(Return *node) {
    auto return_type =
        node->value ? visitExpr(node->value) : BasicTypeKind::Void;

    // order a strict match
    if (GetReturnType() != return_type) {
      e.TypeError(node->loc, "return type mismatched:", "function",
                  Quote(GetFuncName()), "must return",
                  CStringFromBasicTypeKind(GetReturnType()), "not",
                  CStringFromBasicTypeKind(return_type));
    }
  }

  void visitAssign(Assign *node) {
    int errs = e.GetErrorCount();
    auto value = CheckExprOperand(node->value);
    auto target = visitExpr(node->target);

    if (e.IsOk(errs) && target != value) {
      e.TypeError(node->loc, "type mismatched in assignment:",
                  CStringFromBasicTypeKind(target), "=",
                  CStringFromBasicTypeKind(value));
    }
  }

  void visitExprStmt(ExprStmt *node) {
    auto call = subclass_cast<Call>(node->value);
    assert(call && "value of ExprStmt must be a Call");
    visitCall(call);
  }

  // check the operand of BoolOp, restrict to int
  void CheckBoolOpOperand(Expr *operand) {
    auto msg = "operands of condition must be of type int";
    auto errs = e.GetErrorCount();
    auto type = visitExpr(operand);
    if (e.IsOk(errs) && type != BasicTypeKind::Int) {
      e.TypeError(operand->loc, msg);
    }
  }

  BasicTypeKind visitBoolOp(BoolOp *node) {
    if (node->has_cmpop) {
      auto x = static_cast<BinOp *>(node->value);
      CheckBoolOpOperand(x->left);
      CheckBoolOpOperand(x->right);
    } else {
      CheckBoolOpOperand(node->value);
    }
    return BasicTypeKind::Int;
  }

  // check the operand of Expr, restrict to NOT void
  BasicTypeKind CheckExprOperand(Expr *expr) {
    auto msg = "void value cannot be used in an expression";
    auto errs = e.GetErrorCount();
    auto type = visitExpr(expr);
    if (e.IsOk(errs) && type == BasicTypeKind::Void) {
      e.TypeError(expr->loc, msg);
    }
    return type;
  }

  BasicTypeKind visitBinOp(BinOp *node) {
    CheckExprOperand(node->left);
    CheckExprOperand(node->right);
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitUnaryOp(UnaryOp *node) {
    CheckExprOperand(node->operand);
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitParenExpr(ParenExpr *node) {
    CheckExprOperand(node->value);
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitCall(Call *node) {
    const auto &entry = local[node->func];
    if (!entry.IsFunction()) {
      e.TypeError(node->loc, "object of type", entry.GetTypeName(),
                  "cannot be called as a function");
      return BasicTypeKind::Void;
    }

    auto fun_type = entry.AsFunction();
    auto formal_args_len = fun_type.GetArgCount();
    auto actual_args_len = node->args.size();
    if (formal_args_len != actual_args_len) {
      e.TypeError(node->loc, "function", Quote(node->func), "expects",
                  formal_args_len, "arguments, got", actual_args_len);
    }

    // check args
    auto len = std::min(formal_args_len, actual_args_len);
    for (int i = 0; i < len; i++) {
      auto actual = visitExpr(node->args[i]);
      auto formal = fun_type.GetArgTypeAt(i);
      if (actual != formal) {
        e.TypeError(node->args[i]->loc, "argument", i + 1, "of function",
                    Quote(node->func), "must be",
                    CStringFromBasicTypeKind(formal), ", not",
                    CStringFromBasicTypeKind(actual));
      }
    }
    return fun_type.GetReturnType();
  }

  BasicTypeKind visitSubscript(Subscript *node) {
    const auto &entry = local[node->name];
    if (!entry.IsArray()) {
      e.TypeError(node->loc, "object of type", entry.GetTypeName(),
                  "cannot be subscripted as an array");
      return BasicTypeKind::Void;
    }

    int errs = e.GetErrorCount();
    auto index = visitExpr(node->index);
    if (e.IsOk(errs) && index != BasicTypeKind::Int) {
      e.TypeError(node->loc, "type of array index must be int");
    }
    return entry.AsArray().GetElementType();
  }

  BasicTypeKind visitName(Name *node) {
    const auto &entry = local[node->id];
    if (node->ctx == ExprContextKind::Load && entry.IsArray()) {
      e.TypeError(node->loc, "object of type", entry.GetTypeName(),
                  "cannot be used in an expression");
      return BasicTypeKind::Void;
    }
    if (node->ctx == ExprContextKind::Store && !entry.IsVariable()) {
      e.TypeError(node->loc, "object of type", entry.GetTypeName(),
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
    e.InternalError(x->loc, "TypeCheker::visitStr() shall not be called");
  }
  BasicTypeKind visitChar(Char *x) { return BasicTypeKind::Character; }
};
} // namespace

bool simplecompiler::CheckType(Program *prog, SymbolTable &symtable) {
  ErrorManager e;
  for (auto decl : prog->decls) {
    if (auto fun = subclass_cast<FuncDef>(decl)) {
      // first do transformation
      ImplicitCallTransformer(symtable, fun).Transform();
      TypeCheker(symtable, fun, e).Check();
    }
  }
  return e.IsOk();
}
