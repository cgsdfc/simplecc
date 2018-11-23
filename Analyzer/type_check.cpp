#include "type_check.h"
#include "Visitor.h"
#include "error.h"

// Transform Name to Call if it is a function and it is in the context
// of Expr or ExprStmt (Call really).
class ImplicitCallTransformer: public VisitorBase<ImplicitCallTransformer> {
  SymbolTableView_ view;
public:
  ImplicitCallTransformer(SymbolTableView_ view): view(view) {}

  void visitStmt(Stmt *node) {
    VisitorBase::visitStmt<void>(node);
  }

  Expr *visitExpr(Expr *node) {
    if (auto x = subclass_cast<Name>(node)) {
      if (view[x->id].IsFunction()) {
        // replace such a name with a call
        auto call = new Call(x->id, {}, x->loc);
        delete x;
        return call;
      }
      else {
        return node;
      }
    }
    else {
      VisitorBase::visitExpr<void>(node);
      return node;
    }
  }

#define VISIT(name) do { \
  node->name = visitExpr(node->name); \
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
      VISIT(target);
    }
    VISIT(value);
  }

  void visitFor(For *node) {
    visitStmt(node->initial);
    VISIT(condition);
    visitStmt(node->step);
    for (auto s: node->body) {
      visitStmt(s);
    }
  }

  void visitWhile(While *node) {
    VISIT(condition);
    for (auto s: node->body) {
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
    for (auto s: node->body) {
      visitStmt(s);
    }
    for (auto s : node->orelse) {
      visitStmt(s);
    }
  }

  void visitExprStmt(ExprStmt *node) {
    // ExprStmt is special since the name is **unconditionally**
    // a call, as it is required by the Grammar. so there is no
    // lookup with symtable, no visitExpr().
    if (auto x = subclass_cast<Name>(node->value)) {
      node->value = new Call(x->id, {}, x->loc);
      delete x;
    }
    else {
      // visit call as usual
      assert(IsInstance<Call>(node->value));
      visitExpr(node->value);
    }
  }

  void visitBinOp(BinOp *node) {
    VISIT(left);
    VISIT(right);
  }

  void visitBoolOp(BoolOp *node) {
    VISIT(value);
  }

  void visitParenExpr(ParenExpr *node) {
    VISIT(value);
  }

  void visitUnaryOp(UnaryOp *node) {
    VISIT(operand);
  }

  void visitCall(Call *node) {
    for (int i = 0, size = node->args.size(); i < size; i++) {
      node->args[i] = visitExpr(node->args[i]);
    }
  }

  void visitNum(Num*) {}
  void visitStr(Str*) {}
  void visitChar(Char*) {}
  void visitName(Name*) {
    // calling this is an error
    assert(false && "All Names must be handled by container nodes");
  }

  void visitSubscript(Subscript *node) {
    VISIT(index);
  }

  void visitFuncDef(FuncDef *node) {
    for (auto s: node->stmts) {
      visitStmt(s);
    }
  }

#undef VISIT

};

class TypeCheker: public VisitorBase<TypeCheker>,
                  public ChildrenVisitor<TypeCheker> {
public:
  SymbolTableView_ view;
  // point to the entry of the function being checked
  const SymbolEntry &cur_fun;
  ErrorManager &e;

  TypeCheker(SymbolTableView_ view, const SymbolEntry &cur_fun, ErrorManager &e):
    view(view), cur_fun(cur_fun), e(e) {}

  void visitStmt(Stmt *s) {
    return VisitorBase::visitStmt<void>(s);
  }

  // Return the type of evaluating the expression
  BasicTypeKind visitExpr(Expr *node) {
    return VisitorBase::visitExpr<BasicTypeKind>(node);
  }

  void visitFuncDef(FuncDef *node) {
    for (auto stmt: node->stmts) {
      visitStmt(stmt);
    }
  }

  void visitRead(Read *node) {
    for (auto expr: node->names) {
      auto name = subclass_cast<Name>(expr);
      const auto &entry = view[ name->id ];
      if (!entry.IsVariable()) {
        e.TypeError(name->loc,
            "cannot use scanf() on object of type", entry.GetTypeName());
      }
    }
  }

  void visitWrite(Write *node) {
    if (node->value) {
      CheckExprOperand(node->value);
    }
  }

  void visitReturn(Return *node) {
    auto fun_type = cur_fun.AsFunction();
    auto return_type = node->value ? visitExpr(node->value) : BasicTypeKind::Void;

    // order a strict match
    if (fun_type.GetReturnType() != return_type) {
      e.TypeError(node->loc,
          "return type mismatched:", "function", Quote(cur_fun.GetName()),
          "must return", CStringFromBasicTypeKind(fun_type.GetReturnType()),
          "not", CStringFromBasicTypeKind(return_type));
    }
  }

  void visitAssign(Assign *node) {
    int errs = e.GetErrorCount();
    auto value = CheckExprOperand(node->value);
    auto target = visitExpr(node->target);

    // if the assignment target or value is bad, don't check type-match, since:
    // int array[10];
    // array = i;
    // first causes a "object of Array cannot be assigned to",
    // then will cause a "type mismatched in assignment: target type is void,
    // value type is int", but the target type does not support assignment.
    // i = array; it is the same since value is bad.
    if (e.IsOk(errs) && target != value) {
      e.TypeError(node->loc,
          "type mismatched in assignment: target type is",
          CStringFromBasicTypeKind(target), "value type is",
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
    if (auto x = subclass_cast<BinOp>(node->value)) {
      CheckBoolOpOperand(x->left);
      CheckBoolOpOperand(x->right);
    }
    else {
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
    return BasicTypeKind::Int;
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
    const auto &entry = view[ node->func ];
    if (!entry.IsFunction()) {
      e.TypeError(node->loc,
          "object of type", entry.GetTypeName(), "cannot be called as a function");
      return BasicTypeKind::Void;
    }

    auto fun_type = entry.AsFunction();
    auto formal_args_len = fun_type.GetArgCount();
    auto actual_args_len = node->args.size();
    if (formal_args_len != actual_args_len) {
      e.TypeError(node->loc, "function", Quote(node->func),
          "expects", formal_args_len, "arguments, got", actual_args_len);
    }

    // check args
    auto len = std::min(formal_args_len, actual_args_len);
    for (int i = 0; i < len; i++) {
      auto actual = visitExpr(node->args[i]);
      auto formal = fun_type.GetArgTypeAt(i);
      if (actual != formal) {
        e.TypeError(node->args[i]->loc,
            "argument", i + 1, "of function", Quote(node->func),
            "must be", CStringFromBasicTypeKind(formal), ", not",
            CStringFromBasicTypeKind(actual));
      }
    }
    return fun_type.GetReturnType();
  }

  BasicTypeKind visitSubscript(Subscript *node) {
    const auto &entry = view[ node->name ];
    if (!entry.IsArray()) {
      e.TypeError(node->loc,
          "object of type", entry.GetTypeName(), "cannot be subscripted as an array");
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
    const auto &entry = view[ node->id ];
    if (node->ctx == ExprContextKind::Load && entry.IsArray()) {
      e.TypeError(node->loc,
          "object of type", entry.GetTypeName(), "cannot be used in an expression");
      return BasicTypeKind::Void;
    }
    if (node->ctx == ExprContextKind::Store && !entry.IsVariable()) {
      e.TypeError(node->loc,
          "object of type", entry.GetTypeName(), "cannot be assigned to");
      return BasicTypeKind::Void;
    }
    if (entry.IsConstant())
      return entry.AsConstant().GetType();
    return entry.AsVariable().GetType();
  }

  BasicTypeKind visitNum(Num *x) { return BasicTypeKind::Int; }
  // not actually used, for instantiation only
  BasicTypeKind visitStr(Str *x) { return BasicTypeKind::Void; }
  BasicTypeKind visitChar(Char *x) { return BasicTypeKind::Character; }

};

bool CheckType(Program *prog, SymbolTable &symtable) {
  ErrorManager e;
  for (auto decl: prog->decls) {
    if (auto fun = subclass_cast<FuncDef>(decl)) {
      // first do transformation
      ImplicitCallTransformer(symtable.GetLocal(fun->name)).visitFuncDef(fun);
      TypeCheker checker(
          symtable.GetLocal(fun->name),
          symtable.GetGlobal()[fun->name], e);
      checker.visitFuncDef(fun);
    }
  }
  return e.IsOk();
}
