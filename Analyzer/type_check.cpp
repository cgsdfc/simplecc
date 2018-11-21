#include "type_check.h"
#include "Visitor.h"
#include "error.h"


// Transform Name to Call if it is a function and it is in the context
// of Expr or ExprStmt (Call really).
class ImplicitCallTransformer: public VisitorBase<ImplicitCallTransformer> {
  SymbolTable &symtable;
  ErrorManager e;
  // point to the entry of the function being checked
  const Entry *cur_fun;
public:
  ImplicitCallTransformer(SymbolTable &symtable):
    symtable(symtable), e(), cur_fun(nullptr) {}

  // lookup the type of name within the current function
  Type *LookupType(const String &name) {
    assert(cur_fun);
    const auto &entry = symtable.LookupLocal(cur_fun->name, name);
    return entry.type;
  }

  void visitStmt(Stmt *node) {
    VisitorBase::visitStmt<void>(node);
  }

  Expr *visitExpr(Expr *node) {
    if (auto x = subclass_cast<Name>(node)) {
      if (auto fun = subclass_cast<Function>(LookupType(x->id))) {
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
      VISIT(value);
  }

  void visitBinOp(BinOp *node) {
    VISIT(left);
    VISIT(right);
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

  void visitProgam(Program *node) {
    for (auto decl: node->decls) {
      if (auto x = subclass_cast<FuncDef>(decl)) {
        SetCurrentFunction(x->name);
        visitFuncDef(x);
      }
    }
  }

  void SetCurrentFunction(const String &name) {
    cur_fun = &symtable.LookupGlobal(name);
    assert(IsInstance<Function>(cur_fun->type));
  }
#undef VISIT

};

class TypeCheker: public VisitorBase<TypeCheker>, public ChildrenVisitor<TypeCheker> {
public:
  SymbolTable &symtable;
  ErrorManager e;
  // point to the entry of the function being checked
  const Entry *cur_fun;

  TypeCheker(SymbolTable &symtable):
    symtable(symtable), e(), cur_fun(nullptr) {}

  // lookup the type of name within the current function
  Type *LookupType(const String &name) {
    assert(cur_fun);
    const auto &entry = symtable.LookupLocal(cur_fun->name, name);
    return entry.type;
  }

  // public interface
  bool Check(Program *node) {
    visitProgam(node);
    return e.IsOk();
  }

  void SetCurrentFunction(const String &name) {
    cur_fun = &symtable.LookupGlobal(name);
    assert(IsInstance<Function>(cur_fun->type));
  }

  void visitProgam(Program *node) {
    for (auto decl: node->decls) {
      if (auto fun = subclass_cast<FuncDef>(decl)) {
        visitFuncDef(fun);
      }
    }
  }

  void visitStmt(Stmt *s) {
    return VisitorBase::visitStmt<void>(s);
  }

  BasicTypeKind visitExpr(Expr *node) {
    return VisitorBase::visitExpr<BasicTypeKind>(node);
  }

  void visitFuncDef(FuncDef *node) {
    SetCurrentFunction(node->name);
    for (auto stmt: node->stmts) {
      visitStmt(stmt);
    }
  }

  void visitRead(Read *node) {
    for (auto expr: node->names) {
      auto name = subclass_cast<Name>(expr);
      assert(name && "names of Read must be Names");
      auto type = LookupType(name->id);
      if (!IsInstance<Variable>(type)) {
        e.Error(name->loc,
            "cannot read into object of type", type->ClassName());
      }
    }
  }

  void visitReturn(Return *node) {
    auto fun_type = subclass_cast<Function>(cur_fun->type);
    auto return_type = node->value ? visitExpr(node->value) : BasicTypeKind::Void;

    if (fun_type->return_type != return_type) {
      e.Error(node->loc,
          "return type mismatched:", "function", Quote(cur_fun->name),
          "must return", CStringFromBasicTypeKind(fun_type->return_type),
          "not", CStringFromBasicTypeKind(return_type));
    }
  }

  void visitAssign(Assign *node) {
    int errs = e.GetErrorCount();
    auto target = visitExpr(node->target);
    bool bad_target = e.GetErrorCount() > errs;
    auto value = visitExpr(node->value);

    if (!bad_target && target != value) {
      e.Error(node->loc,
          "type mismatched in assignment: target type is",
          CStringFromBasicTypeKind(target), "value type is",
          CStringFromBasicTypeKind(value));
    }
  }

  void visitExprStmt(ExprStmt *node) {
    if (auto name = subclass_cast<Name>(node->value)) {
      // A function name without passing any argument is interpreted as
      // a call to it without argument.
      auto call = new Call(name->id, {}, name->loc);
      delete name;
      node->value = call;
    }
    auto call = subclass_cast<Call>(node->value);
    assert(call && "value of ExprStmt must be a Call");
    visitExpr(call);
  }

  // if char is in BinOp, it will be cast to an int,
  // even both sides are chars. Thus the result must be int.
  BasicTypeKind visitBinOp(BinOp *node) {
    ChildrenVisitor::visitBinOp(node);
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitUnaryOp(UnaryOp *node) {
    ChildrenVisitor::visitUnaryOp(node);
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitParenExpr(ParenExpr *node) {
    ChildrenVisitor::visitParenExpr(node);
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitCall(Call *node) {
    auto type = LookupType(node->func);
    auto fun_type = subclass_cast<Function>(type);
    if (!fun_type) {
      e.Error(node->loc,
          "object of type", type->ClassName(), "cannot be called as a function");
      return BasicTypeKind::Void;
    }

    auto formal_args_len = fun_type->args.size();
    auto actual_args_len = node->args.size();
    if (formal_args_len != actual_args_len) {
      e.Error(node->loc, "function", Quote(node->func),
          "expects", formal_args_len, "arguments, got", actual_args_len);
    }

    // check args
    auto len = std::min(formal_args_len, actual_args_len);
    for (int i = 0; i < len; i++) {
      auto actual = visitExpr(node->args[i]);
      auto formal = fun_type->args[i];
      if (actual != formal) {
        e.Error(node->args[i]->loc,
            "type mismatched of argument", i + 1, "of function", Quote(node->func),
            ", expected", CStringFromBasicTypeKind(formal), "got",
            CStringFromBasicTypeKind(actual));
      }
    }
    return fun_type->return_type;
  }

  BasicTypeKind visitSubscript(Subscript *node) {
    auto type = LookupType(node->name);
    Array *array_type = subclass_cast<Array>(type);
    if (!array_type) {
      e.Error(node->loc,
          "object of type", type->ClassName(), "cannot be subscripted as an array");
      return BasicTypeKind::Void;
    }
    auto index = visitExpr(node->index);
    if (index != BasicTypeKind::Int) {
      e.Error(node->loc, "type of array index must be int");
    }
    return array_type->elemtype;
  }

  BasicTypeKind visitName(Name *node) {
    auto type = LookupType(node->id);
    if (node->ctx == ExprContextKind::Load) {
      if (IsInstance<Function>(type) || IsInstance<Array>(type)) {
        e.Error(node->loc,
            "object of type", type->ClassName(), "cannot be used in an expression");
        return BasicTypeKind::Void;
      }
    }
    else {
      assert(node->ctx == ExprContextKind::Store);
      if (!IsInstance<Variable>(type)) {
        e.Error(node->loc,
            "object of type", type->ClassName(), "cannot be assigned to");
      }
    }
    if (auto x = subclass_cast<Variable>(type)) {
      return x->type;
    }
    if (auto x = subclass_cast<Constant>(type)) {
      return x->type;
    }
    // this is not possible
    return BasicTypeKind::Void;
  }

  BasicTypeKind visitNum(Num *x) { return BasicTypeKind::Int; }
  // not actually used, for instantiation only
  BasicTypeKind visitStr(Str *x) { return BasicTypeKind::Void; }
  BasicTypeKind visitChar(Char *x) { return BasicTypeKind::Character; }

};

bool CheckType(Program *prog, SymbolTable &symtable) {
  return TypeCheker(symtable).Check(prog);
}
