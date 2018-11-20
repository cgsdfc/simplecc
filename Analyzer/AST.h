
#ifndef AST_H
#define AST_H

#include "tokenize.h"

#include <cassert>
#include <iostream>
#include <optional>
#include <vector>

class AST {
public:
  virtual void Format(std::ostream &os) const = 0;
  virtual ~AST() = 0;
  virtual String ClassName() const = 0;
};

inline AST::~AST() {}

inline std::ostream &operator<<(std::ostream &os, const AST *ast) {
  if (ast == nullptr)
    return os << "None";
  ast->Format(os);
  return os;
}

inline std::ostream &operator<<(std::ostream &os, const AST &ast) {
  return os << &ast;
}

// ForwardDecl
class Program;
class Decl;
class ConstDecl;
class VarDecl;
class FuncDef;
class Arg;
class Stmt;
class Read;
class Write;
class Assign;
class For;
class While;
class Return;
class If;
class ExprStmt;
class Expr;
class BinOp;
class UnaryOp;
class Call;
class Num;
class Str;
class Char;
class Subscript;
class Name;

// EnumClass

enum class OperatorKind { Add, Sub, Mult, Div, Eq, NotEq, Lt, LtE, Gt, GtE };

std::ostream &operator<<(std::ostream &os, OperatorKind val);

enum class UnaryopKind { UAdd, USub };

std::ostream &operator<<(std::ostream &os, UnaryopKind val);

enum class ExprContextKind { Load, Store };

std::ostream &operator<<(std::ostream &os, ExprContextKind val);

enum class BasicTypeKind { Int, Character, Void };

std::ostream &operator<<(std::ostream &os, BasicTypeKind val);

// AbstractNode

class Decl : public AST {
public:
  int subclass_tag;
  Location loc;

  Decl(int subclass_tag, Location loc)
      : AST(), subclass_tag(subclass_tag), loc(loc) {}

  enum { ConstDecl, VarDecl, FuncDef };
};

class Stmt : public AST {
public:
  int subclass_tag;
  Location loc;

  Stmt(int subclass_tag, Location loc)
      : AST(), subclass_tag(subclass_tag), loc(loc) {}

  enum { Read, Write, Assign, For, While, Return, If, ExprStmt };
};

class Expr : public AST {
public:
  int subclass_tag;
  Location loc;

  Expr(int subclass_tag, Location loc)
      : AST(), subclass_tag(subclass_tag), loc(loc) {}

  enum { BinOp, UnaryOp, Call, Num, Str, Char, Subscript, Name };
};

// ConcreteNode

class ConstDecl : public Decl {
public:
  BasicTypeKind type;
  std::string name;
  Expr *value;

  ConstDecl(BasicTypeKind type, const std::string &name, Expr *value,
            Location loc)
      : Decl(Decl::ConstDecl, loc), type(type), name(name), value(value) {}

  ~ConstDecl() override;

  String ClassName() const override { return "ConstDecl"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Decl *x) {
    return x->subclass_tag == Decl::ConstDecl;
  }
};

class VarDecl : public Decl {
public:
  BasicTypeKind type;
  int is_array;
  int size;
  std::string name;

  VarDecl(BasicTypeKind type, int is_array, int size, const std::string &name,
          Location loc)
      : Decl(Decl::VarDecl, loc), type(type), is_array(is_array), size(size),
        name(name) {}

  ~VarDecl() override;

  String ClassName() const override { return "VarDecl"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Decl *x) {
    return x->subclass_tag == Decl::VarDecl;
  }
};

class FuncDef : public Decl {
public:
  BasicTypeKind return_type;
  std::string name;
  std::vector<Arg *> args;
  std::vector<Decl *> decls;
  std::vector<Stmt *> stmts;

  FuncDef(BasicTypeKind return_type, const std::string &name,
          const std::vector<Arg *> &args, const std::vector<Decl *> &decls,
          const std::vector<Stmt *> &stmts, Location loc)
      : Decl(Decl::FuncDef, loc), return_type(return_type), name(name),
        args(args), decls(decls), stmts(stmts) {}

  ~FuncDef() override;

  String ClassName() const override { return "FuncDef"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Decl *x) {
    return x->subclass_tag == Decl::FuncDef;
  }
};

class Read : public Stmt {
public:
  std::vector<Expr *> names;

  Read(const std::vector<Expr *> &names, Location loc)
      : Stmt(Stmt::Read, loc), names(names) {}

  ~Read() override;

  String ClassName() const override { return "Read"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Stmt *x) { return x->subclass_tag == Stmt::Read; }
};

class Write : public Stmt {
public:
  Expr *str;
  Expr *value;

  Write(Expr *str, Expr *value, Location loc)
      : Stmt(Stmt::Write, loc), str(str), value(value) {}

  ~Write() override;

  String ClassName() const override { return "Write"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Stmt *x) { return x->subclass_tag == Stmt::Write; }
};

class Assign : public Stmt {
public:
  Expr *target;
  Expr *value;

  Assign(Expr *target, Expr *value, Location loc)
      : Stmt(Stmt::Assign, loc), target(target), value(value) {}

  ~Assign() override;

  String ClassName() const override { return "Assign"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Stmt *x) { return x->subclass_tag == Stmt::Assign; }
};

class For : public Stmt {
public:
  Stmt *initial;
  Expr *condition;
  Stmt *step;
  std::vector<Stmt *> body;

  For(Stmt *initial, Expr *condition, Stmt *step,
      const std::vector<Stmt *> &body, Location loc)
      : Stmt(Stmt::For, loc), initial(initial), condition(condition),
        step(step), body(body) {}

  ~For() override;

  String ClassName() const override { return "For"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Stmt *x) { return x->subclass_tag == Stmt::For; }
};

class While : public Stmt {
public:
  Expr *condition;
  std::vector<Stmt *> body;

  While(Expr *condition, const std::vector<Stmt *> &body, Location loc)
      : Stmt(Stmt::While, loc), condition(condition), body(body) {}

  ~While() override;

  String ClassName() const override { return "While"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Stmt *x) { return x->subclass_tag == Stmt::While; }
};

class Return : public Stmt {
public:
  Expr *value;

  Return(Expr *value, Location loc) : Stmt(Stmt::Return, loc), value(value) {}

  ~Return() override;

  String ClassName() const override { return "Return"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Stmt *x) { return x->subclass_tag == Stmt::Return; }
};

class If : public Stmt {
public:
  Expr *test;
  std::vector<Stmt *> body;
  std::vector<Stmt *> orelse;

  If(Expr *test, const std::vector<Stmt *> &body,
     const std::vector<Stmt *> &orelse, Location loc)
      : Stmt(Stmt::If, loc), test(test), body(body), orelse(orelse) {}

  ~If() override;

  String ClassName() const override { return "If"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Stmt *x) { return x->subclass_tag == Stmt::If; }
};

class ExprStmt : public Stmt {
public:
  Expr *value;

  ExprStmt(Expr *value, Location loc)
      : Stmt(Stmt::ExprStmt, loc), value(value) {}

  ~ExprStmt() override;

  String ClassName() const override { return "ExprStmt"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Stmt *x) {
    return x->subclass_tag == Stmt::ExprStmt;
  }
};

class BinOp : public Expr {
public:
  Expr *left;
  OperatorKind op;
  Expr *right;

  BinOp(Expr *left, OperatorKind op, Expr *right, Location loc)
      : Expr(Expr::BinOp, loc), left(left), op(op), right(right) {}

  ~BinOp() override;

  String ClassName() const override { return "BinOp"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) { return x->subclass_tag == Expr::BinOp; }
};

class UnaryOp : public Expr {
public:
  UnaryopKind op;
  Expr *operand;

  UnaryOp(UnaryopKind op, Expr *operand, Location loc)
      : Expr(Expr::UnaryOp, loc), op(op), operand(operand) {}

  ~UnaryOp() override;

  String ClassName() const override { return "UnaryOp"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) {
    return x->subclass_tag == Expr::UnaryOp;
  }
};

class Call : public Expr {
public:
  std::string func;
  std::vector<Expr *> args;

  Call(const std::string &func, const std::vector<Expr *> &args, Location loc)
      : Expr(Expr::Call, loc), func(func), args(args) {}

  ~Call() override;

  String ClassName() const override { return "Call"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) { return x->subclass_tag == Expr::Call; }
};

class Num : public Expr {
public:
  int n;

  Num(int n, Location loc) : Expr(Expr::Num, loc), n(n) {}

  ~Num() override;

  String ClassName() const override { return "Num"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) { return x->subclass_tag == Expr::Num; }
};

class Str : public Expr {
public:
  std::string s;

  Str(const std::string &s, Location loc) : Expr(Expr::Str, loc), s(s) {}

  ~Str() override;

  String ClassName() const override { return "Str"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) { return x->subclass_tag == Expr::Str; }
};

class Char : public Expr {
public:
  int c;

  Char(int c, Location loc) : Expr(Expr::Char, loc), c(c) {}

  ~Char() override;

  String ClassName() const override { return "Char"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) { return x->subclass_tag == Expr::Char; }
};

class Subscript : public Expr {
public:
  std::string name;
  Expr *index;
  ExprContextKind ctx;

  Subscript(const std::string &name, Expr *index, ExprContextKind ctx,
            Location loc)
      : Expr(Expr::Subscript, loc), name(name), index(index), ctx(ctx) {}

  ~Subscript() override;

  String ClassName() const override { return "Subscript"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) {
    return x->subclass_tag == Expr::Subscript;
  }
};

class Name : public Expr {
public:
  std::string id;
  ExprContextKind ctx;

  Name(const std::string &id, ExprContextKind ctx, Location loc)
      : Expr(Expr::Name, loc), id(id), ctx(ctx) {}

  ~Name() override;

  String ClassName() const override { return "Name"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) { return x->subclass_tag == Expr::Name; }
};

// LeafNode

class Program : public AST {
public:
  std::vector<Decl *> decls;

  Program(const std::vector<Decl *> &decls) : AST(), decls(decls) {}

  ~Program() override;

  String ClassName() const override { return "Program"; }

  void Format(std::ostream &os) const override;
};

class Arg : public AST {
public:
  BasicTypeKind type;
  std::string name;
  Location loc;

  Arg(BasicTypeKind type, const std::string &name, Location loc)
      : AST(), type(type), name(name), loc(loc) {}

  ~Arg() override;

  String ClassName() const override { return "Arg"; }

  void Format(std::ostream &os) const override;
};

// String2Enum

OperatorKind String2OperatorKind(const String &s);

UnaryopKind String2UnaryopKind(const String &s);

BasicTypeKind String2BasicTypeKind(const String &s);

template <typename T, typename U> inline bool IsInstance(U *x) {
  return T::InstanceCheck(x);
}

template <typename T, typename U> inline T *subclass_cast(U *x) {
  if (IsInstance<T>(x))
    return static_cast<T *>(x);
  return nullptr;
}

String GetDeclName(Decl *decl);

// Visitor Mixin that provides runtime dispatch of abstract nodes
template <typename Derived> class VisitorBase {
public:
  template <typename R, typename... Args>
  R visitDecl(Decl *node, Args &&... args) {

    if (auto x = subclass_cast<ConstDecl>(node))
      return static_cast<Derived *>(this)->visitConstDecl(x, args...);

    if (auto x = subclass_cast<VarDecl>(node))
      return static_cast<Derived *>(this)->visitVarDecl(x, args...);

    if (auto x = subclass_cast<FuncDef>(node))
      return static_cast<Derived *>(this)->visitFuncDef(x, args...);

    assert(false && "Decl");
  }

  template <typename R, typename... Args>
  R visitStmt(Stmt *node, Args &&... args) {

    if (auto x = subclass_cast<Read>(node))
      return static_cast<Derived *>(this)->visitRead(x, args...);

    if (auto x = subclass_cast<Write>(node))
      return static_cast<Derived *>(this)->visitWrite(x, args...);

    if (auto x = subclass_cast<Assign>(node))
      return static_cast<Derived *>(this)->visitAssign(x, args...);

    if (auto x = subclass_cast<For>(node))
      return static_cast<Derived *>(this)->visitFor(x, args...);

    if (auto x = subclass_cast<While>(node))
      return static_cast<Derived *>(this)->visitWhile(x, args...);

    if (auto x = subclass_cast<Return>(node))
      return static_cast<Derived *>(this)->visitReturn(x, args...);

    if (auto x = subclass_cast<If>(node))
      return static_cast<Derived *>(this)->visitIf(x, args...);

    if (auto x = subclass_cast<ExprStmt>(node))
      return static_cast<Derived *>(this)->visitExprStmt(x, args...);

    assert(false && "Stmt");
  }

  template <typename R, typename... Args>
  R visitExpr(Expr *node, Args &&... args) {

    if (auto x = subclass_cast<BinOp>(node))
      return static_cast<Derived *>(this)->visitBinOp(x, args...);

    if (auto x = subclass_cast<UnaryOp>(node))
      return static_cast<Derived *>(this)->visitUnaryOp(x, args...);

    if (auto x = subclass_cast<Call>(node))
      return static_cast<Derived *>(this)->visitCall(x, args...);

    if (auto x = subclass_cast<Num>(node))
      return static_cast<Derived *>(this)->visitNum(x, args...);

    if (auto x = subclass_cast<Str>(node))
      return static_cast<Derived *>(this)->visitStr(x, args...);

    if (auto x = subclass_cast<Char>(node))
      return static_cast<Derived *>(this)->visitChar(x, args...);

    if (auto x = subclass_cast<Subscript>(node))
      return static_cast<Derived *>(this)->visitSubscript(x, args...);

    if (auto x = subclass_cast<Name>(node))
      return static_cast<Derived *>(this)->visitName(x, args...);

    assert(false && "Expr");
  }
};

// Visitor Mixin that provides default implementation for visiting nodes
// that have children.
template <class Derived> class ChildrenVisitor {
public:
  template <typename... Args>
  void visitProgram(Program *node, Args &&... args) {

    for (auto s : node->decls) {
      visitDecl(s, args...);
    }
  }

  template <typename... Args>
  void visitConstDecl(ConstDecl *node, Args &&... args) {

    visitExpr(node->value, args...);
  }

  template <typename... Args>
  void visitVarDecl(VarDecl *node, Args &&... args) {}

  template <typename... Args>
  void visitFuncDef(FuncDef *node, Args &&... args) {

    for (auto s : node->args) {
      visitArg(s, args...);
    }

    for (auto s : node->decls) {
      visitDecl(s, args...);
    }

    for (auto s : node->stmts) {
      visitStmt(s, args...);
    }
  }

  template <typename... Args> void visitArg(Arg *node, Args &&... args) {}

  template <typename... Args> void visitRead(Read *node, Args &&... args) {

    for (auto s : node->names) {
      visitExpr(s, args...);
    }
  }

  template <typename... Args> void visitWrite(Write *node, Args &&... args) {

    if (node->str) {
      visitExpr(node->str, args...);
    }

    if (node->value) {
      visitExpr(node->value, args...);
    }
  }

  template <typename... Args> void visitAssign(Assign *node, Args &&... args) {

    visitExpr(node->target, args...);

    visitExpr(node->value, args...);
  }

  template <typename... Args> void visitFor(For *node, Args &&... args) {

    visitStmt(node->initial, args...);

    visitExpr(node->condition, args...);

    visitStmt(node->step, args...);

    for (auto s : node->body) {
      visitStmt(s, args...);
    }
  }

  template <typename... Args> void visitWhile(While *node, Args &&... args) {

    visitExpr(node->condition, args...);

    for (auto s : node->body) {
      visitStmt(s, args...);
    }
  }

  template <typename... Args> void visitReturn(Return *node, Args &&... args) {

    if (node->value) {
      visitExpr(node->value, args...);
    }
  }

  template <typename... Args> void visitIf(If *node, Args &&... args) {

    visitExpr(node->test, args...);

    for (auto s : node->body) {
      visitStmt(s, args...);
    }

    for (auto s : node->orelse) {
      visitStmt(s, args...);
    }
  }

  template <typename... Args>
  void visitExprStmt(ExprStmt *node, Args &&... args) {

    visitExpr(node->value, args...);
  }

  template <typename... Args> void visitBinOp(BinOp *node, Args &&... args) {

    visitExpr(node->left, args...);

    visitExpr(node->right, args...);
  }

  template <typename... Args>
  void visitUnaryOp(UnaryOp *node, Args &&... args) {

    visitExpr(node->operand, args...);
  }

  template <typename... Args> void visitCall(Call *node, Args &&... args) {

    for (auto s : node->args) {
      visitExpr(s, args...);
    }
  }

  template <typename... Args> void visitNum(Num *node, Args &&... args) {}

  template <typename... Args> void visitStr(Str *node, Args &&... args) {}

  template <typename... Args> void visitChar(Char *node, Args &&... args) {}

  template <typename... Args>
  void visitSubscript(Subscript *node, Args &&... args) {

    visitExpr(node->index, args...);
  }

  template <typename... Args> void visitName(Name *node, Args &&... args) {}

  template <typename... Args> void visitDecl(Decl *node, Args &&... args) {
    static_cast<Derived *>(this)->visitDecl(node, args...);
  }

  template <typename... Args> void visitStmt(Stmt *node, Args &&... args) {
    static_cast<Derived *>(this)->visitStmt(node, args...);
  }

  template <typename... Args> void visitExpr(Expr *node, Args &&... args) {
    static_cast<Derived *>(this)->visitExpr(node, args...);
  }
};

#endif
