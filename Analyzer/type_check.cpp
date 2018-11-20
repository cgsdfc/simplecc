#include "type_check.h"
#include "Visitor.h"
#include "error.h"

const char *BasicTypeKind2CString(BasicTypeKind val) {
  switch (val) {
  case BasicTypeKind::Int:
    return "int";
  case BasicTypeKind::Character:
    return "char";
  case BasicTypeKind::Void:
    return "void";
  }
}

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

  void visitExpr(Expr *node, bool void_ok = false) {
    auto type = VisitorBase::visitExpr<BasicTypeKind>(node);
    if (type == BasicTypeKind::Void && !void_ok) {
      e.Error(node->loc, "value of expression cannot be void");
    }
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

  // skip node->str
  void visitWrite(Write *node) {
    if (node->value) {
      visitExpr(node->value);
    }
  }

  void visitReturn(Return *node) {
    auto fun_type = subclass_cast<Function>(cur_fun->type);
    auto return_type = node->value ?
      VisitorBase::visitExpr<BasicTypeKind>(node->value) : BasicTypeKind::Void;

    if ((return_type == BasicTypeKind::Void &&
        fun_type->return_type != BasicTypeKind::Void) ||
       (return_type != BasicTypeKind::Void &&
       fun_type->return_type == BasicTypeKind::Void)) {
      e.Error(node->loc,
          "function", Quote(cur_fun->name), "must return",
          BasicTypeKind2CString(fun_type->return_type), "not",
          BasicTypeKind2CString(return_type));
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
    visitExpr(call, true);
  }

  BasicTypeKind visitBinOp(BinOp *node) {
    visitExpr(node->left);
    visitExpr(node->right);
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitUnaryOp(UnaryOp *node) {
    visitExpr(node->operand);
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitCall(Call *node) {
    auto type = LookupType(node->func);
    auto fun_type = subclass_cast<Function>(type);
    if (!fun_type) {
      e.Error(node->loc,
          "object of type", type->ClassName(), "cannot be called as a function");
      return BasicTypeKind::Int;
    }

    auto formal_args_len = fun_type->args.size();
    auto actual_args_len = node->args.size();
    if (formal_args_len != actual_args_len) {
      e.Error(node->loc, "function", Quote(node->func),
          "expects", formal_args_len, "arguments, got", actual_args_len);
    }

    ChildrenVisitor::visitCall(node);
    return fun_type->return_type;
  }

  BasicTypeKind visitSubscript(Subscript *node) {
    auto type = LookupType(node->name);
    if (auto array_type = subclass_cast<Array>(type); !array_type) {
      e.Error(node->loc,
          "object of type", type->ClassName(), "cannot be subscripted");
    }
    visitExpr(node->index);
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitName(Name *node) {
    auto type = LookupType(node->id);
    if (node->ctx == ExprContextKind::Load) {
      if (IsInstance<Function>(type) || IsInstance<Array>(type)) {
        e.Error(node->loc,
            "object of type", type->ClassName(), "cannot be used in an expression");
      }
    }
    else {
      assert(node->ctx == ExprContextKind::Store);
      if (!IsInstance<Variable>(type)) {
        e.Error(node->loc,
            "object of type", type->ClassName(), "cannot be assigned to");
      }
    }
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitNum(Num *x) { return BasicTypeKind::Int; }
  // not actuall used, for instantiation only
  BasicTypeKind visitStr(Str *x) { return BasicTypeKind::Int; }
  BasicTypeKind visitChar(Char *x) { return BasicTypeKind::Int; }

};

bool CheckType(Program *prog, SymbolTable &symtable) {
  return TypeCheker(symtable).Check(prog);
}
