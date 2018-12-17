
#ifndef AST_H
#define AST_H

#include "simplecompiler/TokenInfo.h"

#include <iostream>
#include <string>
#include <vector>

namespace simplecompiler {
class AST {
public:
  virtual ~AST() = default;
  virtual const char *GetClassName() const = 0;
  virtual void Format(std::ostream &os) const = 0;
};

inline std::ostream &operator<<(std::ostream &os, const AST &ast) {
  ast.Format(os);
  return os;
}

// ForwardDecl
class Program;
class Decl;
class ConstDecl;
class VarDecl;
class FuncDef;
class ArgDecl;
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
  int Kind;

public:
  int GetKind() const { return Kind; }
  std::string name;
  Location loc;

  Decl(int Kind, const std::string &name, const Location &loc)
      : AST(), Kind(Kind), name(name), loc(loc) {}

  enum DeclKind { ConstDecl, VarDecl, FuncDef, ArgDecl };

  const std::string &getName() const { return name; }

  const Location &getLoc() const { return loc; }
};

class Stmt : public AST {
  int Kind;

public:
  int GetKind() const { return Kind; }
  Location loc;

  Stmt(int Kind, const Location &loc) : AST(), Kind(Kind), loc(loc) {}

  enum StmtKind { Read, Write, Assign, For, While, Return, If, ExprStmt };

  const Location &getLoc() const { return loc; }
};

class Expr : public AST {
  int Kind;

public:
  int GetKind() const { return Kind; }
  Location loc;

  Expr(int Kind, const Location &loc) : AST(), Kind(Kind), loc(loc) {}

  enum ExprKind {
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

  const Location &getLoc() const { return loc; }
};

// ConcreteNode

class ConstDecl : public Decl {
public:
  BasicTypeKind type;
  Expr *value;

  ConstDecl(BasicTypeKind type, Expr *value, const std::string &name,
            const Location &loc)
      : Decl(Decl::ConstDecl, name, loc), type(type), value(value) {}

  // Disable copy and move.
  ConstDecl(const ConstDecl &) = delete;
  ConstDecl(ConstDecl &&) = delete;
  ConstDecl &operator=(const ConstDecl &) = delete;
  ConstDecl &operator=(ConstDecl &&) = delete;

  ~ConstDecl() override;

  const char *GetClassName() const override { return "ConstDecl"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Decl *x) { return x->GetKind() == Decl::ConstDecl; }

  BasicTypeKind getType() const { return type; }
  Expr *getValue() const { return value; }
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

  // Disable copy and move.
  VarDecl(const VarDecl &) = delete;
  VarDecl(VarDecl &&) = delete;
  VarDecl &operator=(const VarDecl &) = delete;
  VarDecl &operator=(VarDecl &&) = delete;

  ~VarDecl() override;

  const char *GetClassName() const override { return "VarDecl"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Decl *x) { return x->GetKind() == Decl::VarDecl; }

  BasicTypeKind getType() const { return type; }

  int getIsArray() const { return is_array; }

  int getSize() const { return size; }
};

class FuncDef : public Decl {
public:
  BasicTypeKind return_type;
  std::vector<Decl *> args;
  std::vector<Decl *> decls;
  std::vector<Stmt *> stmts;

  FuncDef(BasicTypeKind return_type, const std::vector<Decl *> &args,
          const std::vector<Decl *> &decls, const std::vector<Stmt *> &stmts,
          const std::string &name, const Location &loc)
      : Decl(Decl::FuncDef, name, loc), return_type(return_type), args(args),
        decls(decls), stmts(stmts) {}

  // Disable copy and move.
  FuncDef(const FuncDef &) = delete;
  FuncDef(FuncDef &&) = delete;
  FuncDef &operator=(const FuncDef &) = delete;
  FuncDef &operator=(FuncDef &&) = delete;

  ~FuncDef() override;

  const char *GetClassName() const override { return "FuncDef"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Decl *x) { return x->GetKind() == Decl::FuncDef; }

  BasicTypeKind getReturnType() const { return return_type; }

  const std::vector<Decl *> &getArgs() const { return args; }

  const std::vector<Decl *> &getDecls() const { return decls; }

  const std::vector<Stmt *> &getStmts() const { return stmts; }
};

class ArgDecl : public Decl {
public:
  BasicTypeKind type;

  ArgDecl(BasicTypeKind type, const std::string &name, const Location &loc)
      : Decl(Decl::ArgDecl, name, loc), type(type) {}

  // Disable copy and move.
  ArgDecl(const ArgDecl &) = delete;
  ArgDecl(ArgDecl &&) = delete;
  ArgDecl &operator=(const ArgDecl &) = delete;
  ArgDecl &operator=(ArgDecl &&) = delete;

  ~ArgDecl() override;

  const char *GetClassName() const override { return "ArgDecl"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Decl *x) { return x->GetKind() == Decl::ArgDecl; }

  BasicTypeKind getType() const { return type; }
};

class Read : public Stmt {
public:
  std::vector<Expr *> names;

  Read(const std::vector<Expr *> &names, const Location &loc)
      : Stmt(Stmt::Read, loc), names(names) {}

  // Disable copy and move.
  Read(const Read &) = delete;
  Read(Read &&) = delete;
  Read &operator=(const Read &) = delete;
  Read &operator=(Read &&) = delete;

  ~Read() override;

  const char *GetClassName() const override { return "Read"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Stmt *x) { return x->GetKind() == Stmt::Read; }

  const std::vector<Expr *> &getNames() const { return names; }
};

class Write : public Stmt {
public:
  Expr *str;
  Expr *value;

  Write(Expr *str, Expr *value, const Location &loc)
      : Stmt(Stmt::Write, loc), str(str), value(value) {}

  // Disable copy and move.
  Write(const Write &) = delete;
  Write(Write &&) = delete;
  Write &operator=(const Write &) = delete;
  Write &operator=(Write &&) = delete;

  ~Write() override;

  const char *GetClassName() const override { return "Write"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Stmt *x) { return x->GetKind() == Stmt::Write; }

  Expr *getStr() const { return str; }
  Expr *getValue() const { return value; }
};

class Assign : public Stmt {
public:
  Expr *target;
  Expr *value;

  Assign(Expr *target, Expr *value, const Location &loc)
      : Stmt(Stmt::Assign, loc), target(target), value(value) {}

  // Disable copy and move.
  Assign(const Assign &) = delete;
  Assign(Assign &&) = delete;
  Assign &operator=(const Assign &) = delete;
  Assign &operator=(Assign &&) = delete;

  ~Assign() override;

  const char *GetClassName() const override { return "Assign"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Stmt *x) { return x->GetKind() == Stmt::Assign; }

  Expr *getTarget() const { return target; }
  Expr *getValue() const { return value; }
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

  // Disable copy and move.
  For(const For &) = delete;
  For(For &&) = delete;
  For &operator=(const For &) = delete;
  For &operator=(For &&) = delete;

  ~For() override;

  const char *GetClassName() const override { return "For"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Stmt *x) { return x->GetKind() == Stmt::For; }

  Stmt *getInitial() const { return initial; }
  Expr *getCondition() const { return condition; }
  Stmt *getStep() const { return step; }

  const std::vector<Stmt *> &getBody() const { return body; }
};

class While : public Stmt {
public:
  Expr *condition;
  std::vector<Stmt *> body;

  While(Expr *condition, const std::vector<Stmt *> &body, const Location &loc)
      : Stmt(Stmt::While, loc), condition(condition), body(body) {}

  // Disable copy and move.
  While(const While &) = delete;
  While(While &&) = delete;
  While &operator=(const While &) = delete;
  While &operator=(While &&) = delete;

  ~While() override;

  const char *GetClassName() const override { return "While"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Stmt *x) { return x->GetKind() == Stmt::While; }

  Expr *getCondition() const { return condition; }

  const std::vector<Stmt *> &getBody() const { return body; }
};

class Return : public Stmt {
public:
  Expr *value;

  Return(Expr *value, const Location &loc)
      : Stmt(Stmt::Return, loc), value(value) {}

  // Disable copy and move.
  Return(const Return &) = delete;
  Return(Return &&) = delete;
  Return &operator=(const Return &) = delete;
  Return &operator=(Return &&) = delete;

  ~Return() override;

  const char *GetClassName() const override { return "Return"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Stmt *x) { return x->GetKind() == Stmt::Return; }

  Expr *getValue() const { return value; }
};

class If : public Stmt {
public:
  Expr *test;
  std::vector<Stmt *> body;
  std::vector<Stmt *> orelse;

  If(Expr *test, const std::vector<Stmt *> &body,
     const std::vector<Stmt *> &orelse, const Location &loc)
      : Stmt(Stmt::If, loc), test(test), body(body), orelse(orelse) {}

  // Disable copy and move.
  If(const If &) = delete;
  If(If &&) = delete;
  If &operator=(const If &) = delete;
  If &operator=(If &&) = delete;

  ~If() override;

  const char *GetClassName() const override { return "If"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Stmt *x) { return x->GetKind() == Stmt::If; }

  Expr *getTest() const { return test; }

  const std::vector<Stmt *> &getBody() const { return body; }

  const std::vector<Stmt *> &getOrelse() const { return orelse; }
};

class ExprStmt : public Stmt {
public:
  Expr *value;

  ExprStmt(Expr *value, const Location &loc)
      : Stmt(Stmt::ExprStmt, loc), value(value) {}

  // Disable copy and move.
  ExprStmt(const ExprStmt &) = delete;
  ExprStmt(ExprStmt &&) = delete;
  ExprStmt &operator=(const ExprStmt &) = delete;
  ExprStmt &operator=(ExprStmt &&) = delete;

  ~ExprStmt() override;

  const char *GetClassName() const override { return "ExprStmt"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Stmt *x) { return x->GetKind() == Stmt::ExprStmt; }

  Expr *getValue() const { return value; }
};

class BinOp : public Expr {
public:
  Expr *left;
  OperatorKind op;
  Expr *right;

  BinOp(Expr *left, OperatorKind op, Expr *right, const Location &loc)
      : Expr(Expr::BinOp, loc), left(left), op(op), right(right) {}

  // Disable copy and move.
  BinOp(const BinOp &) = delete;
  BinOp(BinOp &&) = delete;
  BinOp &operator=(const BinOp &) = delete;
  BinOp &operator=(BinOp &&) = delete;

  ~BinOp() override;

  const char *GetClassName() const override { return "BinOp"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) { return x->GetKind() == Expr::BinOp; }

  Expr *getLeft() const { return left; }
  OperatorKind getOp() const { return op; }
  Expr *getRight() const { return right; }
};

class ParenExpr : public Expr {
public:
  Expr *value;

  ParenExpr(Expr *value, const Location &loc)
      : Expr(Expr::ParenExpr, loc), value(value) {}

  // Disable copy and move.
  ParenExpr(const ParenExpr &) = delete;
  ParenExpr(ParenExpr &&) = delete;
  ParenExpr &operator=(const ParenExpr &) = delete;
  ParenExpr &operator=(ParenExpr &&) = delete;

  ~ParenExpr() override;

  const char *GetClassName() const override { return "ParenExpr"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) { return x->GetKind() == Expr::ParenExpr; }

  Expr *getValue() const { return value; }
};

class BoolOp : public Expr {
public:
  Expr *value;
  int has_cmpop;

  BoolOp(Expr *value, int has_cmpop, const Location &loc)
      : Expr(Expr::BoolOp, loc), value(value), has_cmpop(has_cmpop) {}

  // Disable copy and move.
  BoolOp(const BoolOp &) = delete;
  BoolOp(BoolOp &&) = delete;
  BoolOp &operator=(const BoolOp &) = delete;
  BoolOp &operator=(BoolOp &&) = delete;

  ~BoolOp() override;

  const char *GetClassName() const override { return "BoolOp"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) { return x->GetKind() == Expr::BoolOp; }

  Expr *getValue() const { return value; }

  int getHasCmpop() const { return has_cmpop; }
};

class UnaryOp : public Expr {
public:
  UnaryopKind op;
  Expr *operand;

  UnaryOp(UnaryopKind op, Expr *operand, const Location &loc)
      : Expr(Expr::UnaryOp, loc), op(op), operand(operand) {}

  // Disable copy and move.
  UnaryOp(const UnaryOp &) = delete;
  UnaryOp(UnaryOp &&) = delete;
  UnaryOp &operator=(const UnaryOp &) = delete;
  UnaryOp &operator=(UnaryOp &&) = delete;

  ~UnaryOp() override;

  const char *GetClassName() const override { return "UnaryOp"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) { return x->GetKind() == Expr::UnaryOp; }

  UnaryopKind getOp() const { return op; }
  Expr *getOperand() const { return operand; }
};

class Call : public Expr {
public:
  std::string func;
  std::vector<Expr *> args;

  Call(const std::string &func, const std::vector<Expr *> &args,
       const Location &loc)
      : Expr(Expr::Call, loc), func(func), args(args) {}

  // Disable copy and move.
  Call(const Call &) = delete;
  Call(Call &&) = delete;
  Call &operator=(const Call &) = delete;
  Call &operator=(Call &&) = delete;

  ~Call() override;

  const char *GetClassName() const override { return "Call"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) { return x->GetKind() == Expr::Call; }

  const std::string &getFunc() const { return func; }

  const std::vector<Expr *> &getArgs() const { return args; }
};

class Num : public Expr {
public:
  int n;

  Num(int n, const Location &loc) : Expr(Expr::Num, loc), n(n) {}

  // Disable copy and move.
  Num(const Num &) = delete;
  Num(Num &&) = delete;
  Num &operator=(const Num &) = delete;
  Num &operator=(Num &&) = delete;

  ~Num() override;

  const char *GetClassName() const override { return "Num"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) { return x->GetKind() == Expr::Num; }

  int getN() const { return n; }
};

class Str : public Expr {
public:
  std::string s;

  Str(const std::string &s, const Location &loc) : Expr(Expr::Str, loc), s(s) {}

  // Disable copy and move.
  Str(const Str &) = delete;
  Str(Str &&) = delete;
  Str &operator=(const Str &) = delete;
  Str &operator=(Str &&) = delete;

  ~Str() override;

  const char *GetClassName() const override { return "Str"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) { return x->GetKind() == Expr::Str; }

  const std::string &getS() const { return s; }
};

class Char : public Expr {
public:
  int c;

  Char(int c, const Location &loc) : Expr(Expr::Char, loc), c(c) {}

  // Disable copy and move.
  Char(const Char &) = delete;
  Char(Char &&) = delete;
  Char &operator=(const Char &) = delete;
  Char &operator=(Char &&) = delete;

  ~Char() override;

  const char *GetClassName() const override { return "Char"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) { return x->GetKind() == Expr::Char; }

  int getC() const { return c; }
};

class Subscript : public Expr {
public:
  std::string name;
  Expr *index;
  ExprContextKind ctx;

  Subscript(const std::string &name, Expr *index, ExprContextKind ctx,
            const Location &loc)
      : Expr(Expr::Subscript, loc), name(name), index(index), ctx(ctx) {}

  // Disable copy and move.
  Subscript(const Subscript &) = delete;
  Subscript(Subscript &&) = delete;
  Subscript &operator=(const Subscript &) = delete;
  Subscript &operator=(Subscript &&) = delete;

  ~Subscript() override;

  const char *GetClassName() const override { return "Subscript"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) { return x->GetKind() == Expr::Subscript; }

  const std::string &getName() const { return name; }
  Expr *getIndex() const { return index; }
  ExprContextKind getCtx() const { return ctx; }
};

class Name : public Expr {
public:
  std::string id;
  ExprContextKind ctx;

  Name(const std::string &id, ExprContextKind ctx, const Location &loc)
      : Expr(Expr::Name, loc), id(id), ctx(ctx) {}

  // Disable copy and move.
  Name(const Name &) = delete;
  Name(Name &&) = delete;
  Name &operator=(const Name &) = delete;
  Name &operator=(Name &&) = delete;

  ~Name() override;

  const char *GetClassName() const override { return "Name"; }

  void Format(std::ostream &os) const override;

  static bool InstanceCheck(Expr *x) { return x->GetKind() == Expr::Name; }

  const std::string &getId() const { return id; }
  ExprContextKind getCtx() const { return ctx; }
};

// LeafNode

class Program : public AST {
public:
  std::vector<Decl *> decls;

  Program(const std::vector<Decl *> &decls) : AST(), decls(decls) {}

  // Disable copy and move.
  Program(const Program &) = delete;
  Program(Program &&) = delete;
  Program &operator=(const Program &) = delete;
  Program &operator=(Program &&) = delete;

  ~Program() override;

  const char *GetClassName() const override { return "Program"; }

  void Format(std::ostream &os) const override;

  const std::vector<Decl *> &getDecls() const { return decls; }
};

// EnumFromString

OperatorKind OperatorKindFromString(const String &s);
const char *CStringFromOperatorKind(OperatorKind val);

UnaryopKind UnaryopKindFromString(const String &s);
const char *CStringFromUnaryopKind(UnaryopKind val);

BasicTypeKind BasicTypeKindFromString(const String &s);
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
