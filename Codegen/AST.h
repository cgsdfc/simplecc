
#ifndef AST_H
#define AST_H

#include "tokenize.h"

#include <cassert>
#include <iostream>
#include <optional>
#include <vector>

namespace simplecompiler {
class AST {
public:
  virtual void Format(std::ostream &os) const = 0;
  virtual ~AST() = 0;
  virtual const char *ClassName() const = 0;
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
class ParenExpr;
class BoolOp;
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
  std::string name;
  Location loc;

  Decl(int subclass_tag, const std::string &name, const Location &loc)
      : AST(), subclass_tag(subclass_tag), name(name), loc(loc) {}

  enum { ConstDecl, VarDecl, FuncDef };
};

class Stmt : public AST {
public:
  int subclass_tag;
  Location loc;

  Stmt(int subclass_tag, const Location &loc)
      : AST(), subclass_tag(subclass_tag), loc(loc) {}

  enum { Read, Write, Assign, For, While, Return, If, ExprStmt };
};

class Expr : public AST {
public:
  int subclass_tag;
  Location loc;

  Expr(int subclass_tag, const Location &loc)
      : AST(), subclass_tag(subclass_tag), loc(loc) {}

  enum {
    BinOp,
    ParenExpr,
    BoolOp,
    UnaryOp,
    Call,
    Num,
    Str,
    Char,
    Subscript,
    Name
  };
};

// ConcreteNode

class ConstDecl : public Decl {
public:
  BasicTypeKind type;
  Expr *value;

  ConstDecl(BasicTypeKind type, Expr *value, const std::string &name,
            const Location &loc)
      : Decl(Decl::ConstDecl, name, loc), type(type), value(value) {}

  ~ConstDecl() override;

  const char *ClassName() const override { return "ConstDecl"; }

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

  VarDecl(BasicTypeKind type, int is_array, int size, const std::string &name,
          const Location &loc)
      : Decl(Decl::VarDecl, name, loc), type(type), is_array(is_array),
        size(size) {}

  ~VarDecl() override;

  const char *ClassName() const override { return "VarDecl"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Decl *x) {
    return x->subclass_tag == Decl::VarDecl;
  }
};

class FuncDef : public Decl {
public:
  BasicTypeKind return_type;
  std::vector<Arg *> args;
  std::vector<Decl *> decls;
  std::vector<Stmt *> stmts;

  FuncDef(BasicTypeKind return_type, const std::vector<Arg *> &args,
          const std::vector<Decl *> &decls, const std::vector<Stmt *> &stmts,
          const std::string &name, const Location &loc)
      : Decl(Decl::FuncDef, name, loc), return_type(return_type), args(args),
        decls(decls), stmts(stmts) {}

  ~FuncDef() override;

  const char *ClassName() const override { return "FuncDef"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Decl *x) {
    return x->subclass_tag == Decl::FuncDef;
  }
};

class Read : public Stmt {
public:
  std::vector<Expr *> names;

  Read(const std::vector<Expr *> &names, const Location &loc)
      : Stmt(Stmt::Read, loc), names(names) {}

  ~Read() override;

  const char *ClassName() const override { return "Read"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Stmt *x) { return x->subclass_tag == Stmt::Read; }
};

class Write : public Stmt {
public:
  Expr *str;
  Expr *value;

  Write(Expr *str, Expr *value, const Location &loc)
      : Stmt(Stmt::Write, loc), str(str), value(value) {}

  ~Write() override;

  const char *ClassName() const override { return "Write"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Stmt *x) { return x->subclass_tag == Stmt::Write; }
};

class Assign : public Stmt {
public:
  Expr *target;
  Expr *value;

  Assign(Expr *target, Expr *value, const Location &loc)
      : Stmt(Stmt::Assign, loc), target(target), value(value) {}

  ~Assign() override;

  const char *ClassName() const override { return "Assign"; }

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
      const std::vector<Stmt *> &body, const Location &loc)
      : Stmt(Stmt::For, loc), initial(initial), condition(condition),
        step(step), body(body) {}

  ~For() override;

  const char *ClassName() const override { return "For"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Stmt *x) { return x->subclass_tag == Stmt::For; }
};

class While : public Stmt {
public:
  Expr *condition;
  std::vector<Stmt *> body;

  While(Expr *condition, const std::vector<Stmt *> &body, const Location &loc)
      : Stmt(Stmt::While, loc), condition(condition), body(body) {}

  ~While() override;

  const char *ClassName() const override { return "While"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Stmt *x) { return x->subclass_tag == Stmt::While; }
};

class Return : public Stmt {
public:
  Expr *value;

  Return(Expr *value, const Location &loc)
      : Stmt(Stmt::Return, loc), value(value) {}

  ~Return() override;

  const char *ClassName() const override { return "Return"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Stmt *x) { return x->subclass_tag == Stmt::Return; }
};

class If : public Stmt {
public:
  Expr *test;
  std::vector<Stmt *> body;
  std::vector<Stmt *> orelse;

  If(Expr *test, const std::vector<Stmt *> &body,
     const std::vector<Stmt *> &orelse, const Location &loc)
      : Stmt(Stmt::If, loc), test(test), body(body), orelse(orelse) {}

  ~If() override;

  const char *ClassName() const override { return "If"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Stmt *x) { return x->subclass_tag == Stmt::If; }
};

class ExprStmt : public Stmt {
public:
  Expr *value;

  ExprStmt(Expr *value, const Location &loc)
      : Stmt(Stmt::ExprStmt, loc), value(value) {}

  ~ExprStmt() override;

  const char *ClassName() const override { return "ExprStmt"; }

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

  BinOp(Expr *left, OperatorKind op, Expr *right, const Location &loc)
      : Expr(Expr::BinOp, loc), left(left), op(op), right(right) {}

  ~BinOp() override;

  const char *ClassName() const override { return "BinOp"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) { return x->subclass_tag == Expr::BinOp; }
};

class ParenExpr : public Expr {
public:
  Expr *value;

  ParenExpr(Expr *value, const Location &loc)
      : Expr(Expr::ParenExpr, loc), value(value) {}

  ~ParenExpr() override;

  const char *ClassName() const override { return "ParenExpr"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) {
    return x->subclass_tag == Expr::ParenExpr;
  }
};

class BoolOp : public Expr {
public:
  Expr *value;

  BoolOp(Expr *value, const Location &loc)
      : Expr(Expr::BoolOp, loc), value(value) {}

  ~BoolOp() override;

  const char *ClassName() const override { return "BoolOp"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) { return x->subclass_tag == Expr::BoolOp; }
};

class UnaryOp : public Expr {
public:
  UnaryopKind op;
  Expr *operand;

  UnaryOp(UnaryopKind op, Expr *operand, const Location &loc)
      : Expr(Expr::UnaryOp, loc), op(op), operand(operand) {}

  ~UnaryOp() override;

  const char *ClassName() const override { return "UnaryOp"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) {
    return x->subclass_tag == Expr::UnaryOp;
  }
};

class Call : public Expr {
public:
  std::string func;
  std::vector<Expr *> args;

  Call(const std::string &func, const std::vector<Expr *> &args,
       const Location &loc)
      : Expr(Expr::Call, loc), func(func), args(args) {}

  ~Call() override;

  const char *ClassName() const override { return "Call"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) { return x->subclass_tag == Expr::Call; }
};

class Num : public Expr {
public:
  int n;

  Num(int n, const Location &loc) : Expr(Expr::Num, loc), n(n) {}

  ~Num() override;

  const char *ClassName() const override { return "Num"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) { return x->subclass_tag == Expr::Num; }
};

class Str : public Expr {
public:
  std::string s;

  Str(const std::string &s, const Location &loc) : Expr(Expr::Str, loc), s(s) {}

  ~Str() override;

  const char *ClassName() const override { return "Str"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) { return x->subclass_tag == Expr::Str; }
};

class Char : public Expr {
public:
  int c;

  Char(int c, const Location &loc) : Expr(Expr::Char, loc), c(c) {}

  ~Char() override;

  const char *ClassName() const override { return "Char"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) { return x->subclass_tag == Expr::Char; }
};

class Subscript : public Expr {
public:
  std::string name;
  Expr *index;
  ExprContextKind ctx;

  Subscript(const std::string &name, Expr *index, ExprContextKind ctx,
            const Location &loc)
      : Expr(Expr::Subscript, loc), name(name), index(index), ctx(ctx) {}

  ~Subscript() override;

  const char *ClassName() const override { return "Subscript"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) {
    return x->subclass_tag == Expr::Subscript;
  }
};

class Name : public Expr {
public:
  std::string id;
  ExprContextKind ctx;

  Name(const std::string &id, ExprContextKind ctx, const Location &loc)
      : Expr(Expr::Name, loc), id(id), ctx(ctx) {}

  ~Name() override;

  const char *ClassName() const override { return "Name"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) { return x->subclass_tag == Expr::Name; }
};

// LeafNode

class Program : public AST {
public:
  std::vector<Decl *> decls;

  Program(const std::vector<Decl *> &decls) : AST(), decls(decls) {}

  ~Program() override;

  const char *ClassName() const override { return "Program"; }

  void Format(std::ostream &os) const override;
};

class Arg : public AST {
public:
  BasicTypeKind type;
  std::string name;
  Location loc;

  Arg(BasicTypeKind type, const std::string &name, const Location &loc)
      : AST(), type(type), name(name), loc(loc) {}

  ~Arg() override;

  const char *ClassName() const override { return "Arg"; }

  void Format(std::ostream &os) const override;
};

// String2Enum

OperatorKind String2OperatorKind(const String &s);
const char *CStringFromOperatorKind(OperatorKind val);

UnaryopKind String2UnaryopKind(const String &s);
const char *CStringFromUnaryopKind(UnaryopKind val);

BasicTypeKind String2BasicTypeKind(const String &s);
const char *CStringFromBasicTypeKind(BasicTypeKind val);

template <typename T, typename U> inline bool IsInstance(U *x) {
  return T::InstanceCheck(x);
}

template <typename T, typename U> inline T *subclass_cast(U *x) {
  if (IsInstance<T>(x))
    return static_cast<T *>(x);
  return nullptr;
}
} // namespace simplecompiler
#endif
