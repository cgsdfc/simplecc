#ifndef SIMPLECC_PARSE_AST_H
#define SIMPLECC_PARSE_AST_H
#include "simplecc/Lex/TokenInfo.h"

#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace simplecc {
class AST {
  Location Loc;
  unsigned SubclassID;
protected:
  AST(unsigned Kind, Location L) : SubclassID(Kind), Loc(L) {}
public:
  ~AST() = default;
  enum ASTKind : unsigned {
#define HANDLE_AST(CLASS) CLASS##Kind,
#include "simplecc/Parse/AST.def"
  };
  static const char *getClassName(unsigned Kind);
  const char *getClassName() const { return getClassName(getKind()); }
  unsigned getKind() const { return SubclassID; }
  Location getLocation() const { return Loc; }
  void deleteAST();
  void Format(std::ostream &os) const;
};

inline std::ostream &operator<<(std::ostream &O, const AST &A) {
  A.Format(O);
  return O;
}

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
protected:
  Decl(unsigned int Kind, std::string name, Location loc)
      : AST(Kind, loc), name(std::move(name)) {}
public:
  std::string name;
  const std::string &getName() const { return name; }
  static bool InstanceCheck(AST *A) { return A->getKind() == AST::DeclKind; }
};

class Stmt : public AST {
protected:
  Stmt(unsigned int Kind, Location loc) : AST(Kind, loc) {}
public:
  static bool InstanceCheck(AST *A) { return A->getKind() == AST::StmtKind; }
};

class Expr : public AST {
protected:
  Expr(int Kind, Location loc) : AST(Kind, loc) {}
public:
  static bool InstanceCheck(AST *A) { return A->getKind() == AST::ExprKind; }
};

// ConcreteNode
class ConstDecl : public Decl {
  BasicTypeKind type;
  Expr *value;
public:
  ConstDecl(BasicTypeKind type, Expr *value, std::string name,
            const Location &loc)
      : Decl(Decl::ConstDeclKind, std::move(name), loc), type(type),
        value(value) {}
  ~ConstDecl() { value->deleteAST(); }

  // Disable copy and move.
  ConstDecl(const ConstDecl &) = delete;
  ConstDecl(ConstDecl &&) = delete;
  ConstDecl &operator=(const ConstDecl &) = delete;

  ConstDecl &operator=(ConstDecl &&) = delete;

  static bool InstanceCheck(Decl *x) {
    return x->getKind() == Decl::ConstDeclKind;
  }

  BasicTypeKind getType() const { return type; }
  Expr *getValue() const { return value; }
};

class VarDecl : public Decl {
  BasicTypeKind type;
  int is_array;
  int size;

public:
  VarDecl(BasicTypeKind type, int is_array, int size, std::string name,
          const Location &loc)
      : Decl(Decl::VarDeclKind, std::move(name), loc), type(type),
        is_array(is_array), size(size) {}

  // Disable copy and move.
  VarDecl(const VarDecl &) = delete;
  VarDecl(VarDecl &&) = delete;
  VarDecl &operator=(const VarDecl &) = delete;
  VarDecl &operator=(VarDecl &&) = delete;

  ~VarDecl() = default;

  static bool InstanceCheck(Decl *x) {
    return x->getKind() == Decl::VarDeclKind;
  }

  BasicTypeKind getType() const { return type; }

  int getIsArray() const { return is_array; }

  int getSize() const { return size; }
};

class FuncDef : public Decl {
  BasicTypeKind return_type;
  std::vector<Decl *> args;
  std::vector<Decl *> decls;
  std::vector<Stmt *> stmts;

public:
  FuncDef(BasicTypeKind return_type, std::vector<Decl *> args,
          std::vector<Decl *> decls, std::vector<Stmt *> stmts,
          std::string name, const Location &loc)
      : Decl(Decl::FuncDefKind, std::move(name), loc), return_type(return_type),
        args(std::move(args)), decls(std::move(decls)),
        stmts(std::move(stmts)) {}

  // Disable copy and move.
  FuncDef(const FuncDef &) = delete;
  FuncDef(FuncDef &&) = delete;
  FuncDef &operator=(const FuncDef &) = delete;
  FuncDef &operator=(FuncDef &&) = delete;

  ~FuncDef();

  static bool InstanceCheck(Decl *x) {
    return x->getKind() == Decl::FuncDefKind;
  }

  BasicTypeKind getReturnType() const { return return_type; }
  void setReturnType(BasicTypeKind RetTy) { return_type = RetTy; }

  const std::vector<Decl *> &getArgs() const { return args; }

  const std::vector<Decl *> &getDecls() const { return decls; }

  const std::vector<Stmt *> &getStmts() const { return stmts; }
};

class ArgDecl : public Decl {
  BasicTypeKind type;

public:
  ArgDecl(BasicTypeKind type, std::string name, const Location &loc)
      : Decl(Decl::ArgDeclKind, std::move(name), loc), type(type) {}

  // Disable copy and move.
  ArgDecl(const ArgDecl &) = delete;
  ArgDecl(ArgDecl &&) = delete;
  ArgDecl &operator=(const ArgDecl &) = delete;
  ArgDecl &operator=(ArgDecl &&) = delete;

  ~ArgDecl() = default;

  static bool InstanceCheck(Decl *x) {
    return x->getKind() == Decl::ArgDeclKind;
  }

  BasicTypeKind getType() const { return type; }
};

class ReadStmt : public Stmt {
  std::vector<Expr *> names;

public:
  ReadStmt(std::vector<Expr *> names, const Location &loc)
      : Stmt(Stmt::ReadStmtKind, loc), names(std::move(names)) {}

  // Disable copy and move.
  ReadStmt(const ReadStmt &) = delete;
  ReadStmt(ReadStmt &&) = delete;
  ReadStmt &operator=(const ReadStmt &) = delete;
  ReadStmt &operator=(ReadStmt &&) = delete;

  ~ReadStmt();

  static bool InstanceCheck(Stmt *x) {
    return x->getKind() == Stmt::ReadStmtKind;
  }

  const std::vector<Expr *> &getNames() const { return names; }
};

class WriteStmt : public Stmt {
  Expr *str;
  Expr *value;

public:
  WriteStmt(Expr *str, Expr *value, const Location &loc)
      : Stmt(Stmt::WriteStmtKind, loc), str(str), value(value) {}

  // Disable copy and move.
  WriteStmt(const WriteStmt &) = delete;
  WriteStmt(WriteStmt &&) = delete;
  WriteStmt &operator=(const WriteStmt &) = delete;
  WriteStmt &operator=(WriteStmt &&) = delete;

  ~WriteStmt();

  static bool InstanceCheck(Stmt *x) { return x->getKind() == WriteStmtKind; }

  Expr *getStr() const { return str; }
  Expr *getValue() const { return value; }
  Expr *&getValue() { return value; }
  void setValue(Expr *Val);
};

class AssignStmt : public Stmt {
  Expr *target;
  Expr *value;

public:
  AssignStmt(Expr *target, Expr *value, const Location &loc)
      : Stmt(Stmt::AssignStmtKind, loc), target(target), value(value) {}

  // Disable copy and move.
  AssignStmt(const AssignStmt &) = delete;
  AssignStmt(AssignStmt &&) = delete;
  AssignStmt &operator=(const AssignStmt &) = delete;
  AssignStmt &operator=(AssignStmt &&) = delete;

  ~AssignStmt();

  static bool InstanceCheck(Stmt *x) { return x->getKind() == AssignStmtKind; }

  Expr *getTarget() const { return target; }
  Expr *getValue() const { return value; }
  void setValue(Expr *E);
};

class ForStmt : public Stmt {
  Stmt *initial;
  Expr *condition;
  Stmt *step;
  std::vector<Stmt *> body;

public:
  ForStmt(Stmt *initial, Expr *condition, Stmt *step, std::vector<Stmt *> body,
          const Location &loc)
      : Stmt(Stmt::ForStmtKind, loc), initial(initial), condition(condition),
        step(step), body(std::move(body)) {}

  // Disable copy and move.
  ForStmt(const ForStmt &) = delete;
  ForStmt(ForStmt &&) = delete;
  ForStmt &operator=(const ForStmt &) = delete;
  ForStmt &operator=(ForStmt &&) = delete;

  ~ForStmt();

  static bool InstanceCheck(Stmt *x) {
    return x->getKind() == Stmt::ForStmtKind;
  }

  Stmt *getInitial() const { return initial; }

  Expr *getCondition() const { return condition; }
  void setCondition(Expr *E);

  Stmt *getStep() const { return step; }

  const std::vector<Stmt *> &getBody() const { return body; }
};

class WhileStmt : public Stmt {
  Expr *condition;
  std::vector<Stmt *> body;

public:
  WhileStmt(Expr *condition, std::vector<Stmt *> body, const Location &loc)
      : Stmt(WhileStmtKind, loc), condition(condition), body(std::move(body)) {}

  // Disable copy and move.
  WhileStmt(const WhileStmt &) = delete;
  WhileStmt(WhileStmt &&) = delete;
  WhileStmt &operator=(const WhileStmt &) = delete;
  WhileStmt &operator=(WhileStmt &&) = delete;

  ~WhileStmt();

  static bool InstanceCheck(Stmt *x) { return x->getKind() == WhileStmtKind; }

  Expr *getCondition() const { return condition; }
  void setCondition(Expr *E);

  const std::vector<Stmt *> &getBody() const { return body; }
};

class ReturnStmt : public Stmt {
  Expr *value;

public:
  ReturnStmt(Expr *value, const Location &loc)
      : Stmt(Stmt::ReturnStmtKind, loc), value(value) {}

  // Disable copy and move.
  ReturnStmt(const ReturnStmt &) = delete;
  ReturnStmt(ReturnStmt &&) = delete;
  ReturnStmt &operator=(const ReturnStmt &) = delete;
  ReturnStmt &operator=(ReturnStmt &&) = delete;

  ~ReturnStmt();

  static bool InstanceCheck(Stmt *x) {
    return x->getKind() == Stmt::ReturnStmtKind;
  }

  Expr *getValue() const { return value; }
  void setValue(Expr *E);
};

class IfStmt : public Stmt {
  Expr *test;
  std::vector<Stmt *> body;
  std::vector<Stmt *> orelse;

public:
  IfStmt(Expr *test, std::vector<Stmt *> body, std::vector<Stmt *> orelse,
         const Location &loc)
      : Stmt(Stmt::IfStmtKind, loc), test(test), body(std::move(body)),
        orelse(std::move(orelse)) {}

  // Disable copy and move.
  IfStmt(const IfStmt &) = delete;
  IfStmt(IfStmt &&) = delete;
  IfStmt &operator=(const IfStmt &) = delete;
  IfStmt &operator=(IfStmt &&) = delete;

  ~IfStmt();

  static bool InstanceCheck(Stmt *x) {
    return x->getKind() == Stmt::IfStmtKind;
  }

  Expr *getTest() const { return test; }
  void setTest(Expr *E);

  const std::vector<Stmt *> &getBody() const { return body; }
  const std::vector<Stmt *> &getOrelse() const { return orelse; }
};

class ExprStmt : public Stmt {
  Expr *value;

public:
  ExprStmt(Expr *value, const Location &loc)
      : Stmt(Stmt::ExprStmtKind, loc), value(value) {}

  // Disable copy and move.
  ExprStmt(const ExprStmt &) = delete;
  ExprStmt(ExprStmt &&) = delete;
  ExprStmt &operator=(const ExprStmt &) = delete;
  ExprStmt &operator=(ExprStmt &&) = delete;

  ~ExprStmt();

  static bool InstanceCheck(Stmt *x) {
    return x->getKind() == Stmt::ExprStmtKind;
  }

  Expr *getValue() const { return value; }
  void setValue(Expr *E);
};

class BinOpExpr : public Expr {
  Expr *left;
  OperatorKind op;
  Expr *right;

public:
  BinOpExpr(Expr *left, OperatorKind op, Expr *right, const Location &loc)
      : Expr(Expr::BinOpExprKind, loc), left(left), op(op), right(right) {}

  // Disable copy and move.
  BinOpExpr(const BinOpExpr &) = delete;
  BinOpExpr(BinOpExpr &&) = delete;
  BinOpExpr &operator=(const BinOpExpr &) = delete;
  BinOpExpr &operator=(BinOpExpr &&) = delete;

  ~BinOpExpr();

  static bool InstanceCheck(Expr *x) {
    return x->getKind() == Expr::BinOpExprKind;
  }

  OperatorKind getOp() const { return op; }

  Expr *getLeft() const { return left; }
  void setLeft(Expr *E);

  Expr *getRight() const { return right; }
  void setRight(Expr *E);
};

class ParenExpr : public Expr {
  Expr *value;

public:
  ParenExpr(Expr *value, const Location &loc)
      : Expr(Expr::ParenExprKind, loc), value(value) {}

  // Disable copy and move.
  ParenExpr(const ParenExpr &) = delete;
  ParenExpr(ParenExpr &&) = delete;
  ParenExpr &operator=(const ParenExpr &) = delete;
  ParenExpr &operator=(ParenExpr &&) = delete;

  ~ParenExpr();

  static bool InstanceCheck(Expr *x) {
    return x->getKind() == Expr::ParenExprKind;
  }

  Expr *getValue() const { return value; }
  void setValue(Expr *E);
};

class BoolOpExpr : public Expr {
  Expr *value;
  int has_cmpop;

public:
  BoolOpExpr(Expr *value, int has_cmpop, const Location &loc)
      : Expr(BoolOpExprKind, loc), value(value), has_cmpop(has_cmpop) {}

  // Disable copy and move.
  BoolOpExpr(const BoolOpExpr &) = delete;
  BoolOpExpr(BoolOpExpr &&) = delete;
  BoolOpExpr &operator=(const BoolOpExpr &) = delete;
  BoolOpExpr &operator=(BoolOpExpr &&) = delete;

  ~BoolOpExpr();

  static bool InstanceCheck(const Expr *x) { return x->getKind() == BoolOpExprKind; }

  Expr *getValue() const { return value; }
  void setValue(Expr *E);

  int getHasCmpop() const { return has_cmpop; }
};

class UnaryOpExpr : public Expr {
  UnaryopKind op;
  Expr *operand;

public:
  UnaryOpExpr(UnaryopKind op, Expr *operand, const Location &loc)
      : Expr(UnaryOpExprKind, loc), op(op), operand(operand) {}

  // Disable copy and move.
  UnaryOpExpr(const UnaryOpExpr &) = delete;
  UnaryOpExpr(UnaryOpExpr &&) = delete;
  UnaryOpExpr &operator=(const UnaryOpExpr &) = delete;
  UnaryOpExpr &operator=(UnaryOpExpr &&) = delete;

  ~UnaryOpExpr();

  static bool InstanceCheck(const Expr *x) {
    return x->getKind() == Expr::UnaryOpExprKind;
  }

  UnaryopKind getOp() const { return op; }
  Expr *getOperand() const { return operand; }
  void setOperand(Expr *E);
};

class CallExpr : public Expr {
  std::string func;
  std::vector<Expr *> args;

public:
  CallExpr(std::string func, std::vector<Expr *> args, const Location &loc)
      : Expr(Expr::CallExprKind, loc), func(std::move(func)),
        args(std::move(args)) {}

  // Disable copy and move.
  CallExpr(const CallExpr &) = delete;
  CallExpr(CallExpr &&) = delete;
  CallExpr &operator=(const CallExpr &) = delete;
  CallExpr &operator=(CallExpr &&) = delete;

  ~CallExpr();

  static bool InstanceCheck(const Expr *x) {
    return x->getKind() == Expr::CallExprKind;
  }

  const std::string &getFunc() const { return func; }

  const std::vector<Expr *> &getArgs() const { return args; }
  Expr *getArgAt(unsigned I) const { return args[I]; }
  void setArgAt(unsigned I, Expr *Val);
  unsigned getNumArgs() const { return args.size(); }
};

class NumExpr : public Expr {
  int n;

public:
  NumExpr(int n, const Location &loc) : Expr(Expr::NumExprKind, loc), n(n) {}

  // Disable copy and move.
  NumExpr(const NumExpr &) = delete;
  NumExpr(NumExpr &&) = delete;
  NumExpr &operator=(const NumExpr &) = delete;
  NumExpr &operator=(NumExpr &&) = delete;

  static bool InstanceCheck(const Expr *x) {
    return x->getKind() == Expr::NumExprKind;
  }

  int getN() const { return n; }
};

class StrExpr : public Expr {
  std::string s;

public:
  StrExpr(std::string s, const Location &loc)
      : Expr(Expr::StrExprKind, loc), s(std::move(s)) {}

  // Disable copy and move.
  StrExpr(const StrExpr &) = delete;
  StrExpr(StrExpr &&) = delete;
  StrExpr &operator=(const StrExpr &) = delete;
  StrExpr &operator=(StrExpr &&) = delete;

  static bool InstanceCheck(const Expr *x) {
    return x->getKind() == Expr::StrExprKind;
  }

  const std::string &getS() const { return s; }
};

class CharExpr : public Expr {
  int c;

public:
  CharExpr(int c, const Location &loc) : Expr(Expr::CharExprKind, loc), c(c) {}

  // Disable copy and move.
  CharExpr(const CharExpr &) = delete;
  CharExpr(CharExpr &&) = delete;
  CharExpr &operator=(const CharExpr &) = delete;
  CharExpr &operator=(CharExpr &&) = delete;

  static bool InstanceCheck(const Expr *x) {
    return x->getKind() == Expr::CharExprKind;
  }

  int getC() const { return c; }
};

class SubscriptExpr : public Expr {
  std::string name;
  Expr *index;
  ExprContextKind ctx;

public:
  SubscriptExpr(std::string name, Expr *index, ExprContextKind ctx,
                const Location &loc)
      : Expr(SubscriptExprKind, loc), name(std::move(name)), index(index),
        ctx(ctx) {}

  // Disable copy and move.
  SubscriptExpr(const SubscriptExpr &) = delete;
  SubscriptExpr(SubscriptExpr &&) = delete;
  SubscriptExpr &operator=(const SubscriptExpr &) = delete;
  SubscriptExpr &operator=(SubscriptExpr &&) = delete;

  ~SubscriptExpr();

  static bool InstanceCheck(const Expr *x) {
    return x->getKind() == SubscriptExprKind;
  }

  const std::string &getName() const { return name; }
  ExprContextKind getCtx() const { return ctx; }

  Expr *getIndex() const { return index; }
  void setIndex(Expr *E);
};

class NameExpr : public Expr {
  std::string id;
  ExprContextKind ctx;

public:
  NameExpr(std::string id, ExprContextKind ctx, const Location &loc)
      : Expr(NameExprKind, loc), id(std::move(id)), ctx(ctx) {}

  // Disable copy and move.
  NameExpr(const NameExpr &) = delete;
  NameExpr(NameExpr &&) = delete;
  NameExpr &operator=(const NameExpr &) = delete;
  NameExpr &operator=(NameExpr &&) = delete;

  static bool InstanceCheck(const Expr *x) { return x->getKind() == NameExprKind; }

  const std::string &getId() const { return id; }
  ExprContextKind getCtx() const { return ctx; }
};

// LeafNode

class Program : public AST {
  std::vector<Decl *> decls;
public:
  ~Program();
  explicit Program(std::vector<Decl *> decls)
      : AST(ProgramKind, Location(0, 0)), decls(std::move(decls)) {}

  // Disable copy and move.
  Program(const Program &) = delete;
  Program(Program &&) = delete;
  Program &operator=(const Program &) = delete;
  Program &operator=(Program &&) = delete;

  const std::vector<Decl *> &getDecls() const { return decls; }

  static bool InstanceCheck(const AST *A) { return A->getKind() == ProgramKind; }
};

// EnumFromString
OperatorKind OperatorKindFromString(const String &s);
const char *CStringFromOperatorKind(OperatorKind val);

UnaryopKind UnaryopKindFromString(const String &s);
const char *CStringFromUnaryopKind(UnaryopKind val);

BasicTypeKind BasicTypeKindFromString(const String &s);
const char *CStringFromBasicTypeKind(BasicTypeKind val);

/// To be used with std::unique_ptr.
struct DeleteAST {
  static void apply(AST *A) {
    if (A)
      A->deleteAST();
  }
  void operator()(AST *A) const {
    apply(A);
  }
};

} // namespace simplecc
#endif
