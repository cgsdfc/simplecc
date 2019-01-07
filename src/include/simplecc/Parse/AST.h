#ifndef SIMPLECC_PARSE_AST_H
#define SIMPLECC_PARSE_AST_H
#include "simplecc/Lex/Location.h"
#include "simplecc/Parse/Enums.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace simplecc {
class AST {
  unsigned SubclassID;
  Location Loc;

protected:
  AST(unsigned Kind, Location L) : SubclassID(Kind), Loc(L) {}
  ~AST() = default;

public:
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
  void dump() const;
};

inline std::ostream &operator<<(std::ostream &O, const AST &A) {
  A.Format(O);
  return O;
}

/// This struct knows how to delete an AST or a list of AST.
struct DeleteAST {
  /// Delete a range of AST.
  template<typename Iterator>
  static void apply(Iterator first, Iterator last) {
    std::for_each(first, last, DeleteAST());
  }
  /// Delete a vector of AST, assuming the vector own them.
  template<typename AstT> static void apply(const std::vector<AstT *> &List) {
    apply(List.begin(), List.end());
  }
  /// Delete a single AST.
  static void apply(AST *A) {
    if (A)
      A->deleteAST();
  }
  // As a functor.
  void operator()(AST *A) const { apply(A); }
};

using UniquePtrToAST = std::unique_ptr<AST, DeleteAST>;

// AbstractNode
class DeclAST : public AST {
  std::string name;

protected:
  DeclAST(unsigned int Kind, std::string name, Location loc)
      : AST(Kind, loc), name(std::move(name)) {}

public:
  const std::string &getName() const &{ return name; }
  static bool InstanceCheck(const AST *A);
};

class StmtAST : public AST {
protected:
  StmtAST(unsigned int Kind, Location loc) : AST(Kind, loc) {}

public:
  using StmtListType = std::vector<StmtAST *>;
  static bool InstanceCheck(const AST *A);
};

class ExprAST : public AST {
protected:
  ExprAST(unsigned Kind, Location loc) : AST(Kind, loc) {}
  bool isConstantImpl() const { return false; }
  int getConstantValueImpl() const {
    assert(false && "subclass should implement this!");
    return 0; // for MSVC to be happy.
  }

public:
  bool isConstant() const;
  int getConstantValue() const;
  bool isZeroVal() const { return isConstant() && 0 == getConstantValue(); }
  bool isOneVal() const { return isConstant() && 1 == getConstantValue(); }
  static bool InstanceCheck(const AST *A);
};

// ConcreteNode
class ConstDecl : public DeclAST {
  friend class AST;
  BasicTypeKind type;
  ExprAST *value;
  ~ConstDecl() { value->deleteAST(); }

public:
  ConstDecl(BasicTypeKind type, ExprAST *value, std::string name,
            Location loc)
      : DeclAST(DeclAST::ConstDeclKind, std::move(name), loc), type(type),
        value(value) {}

  // Disable copy and move.
  ConstDecl(const ConstDecl &) = delete;
  ConstDecl(ConstDecl &&) = delete;
  ConstDecl &operator=(const ConstDecl &) = delete;
  ConstDecl &operator=(ConstDecl &&) = delete;

  BasicTypeKind getType() const { return type; }
  ExprAST *getValue() const { return value; }

  static bool InstanceCheck(const DeclAST *x) {
    return x->getKind() == DeclAST::ConstDeclKind;
  }
};

class VarDecl : public DeclAST {
  friend class AST;
  BasicTypeKind type;
  bool is_array;
  int size;
  ~VarDecl() = default;

public:
  VarDecl(BasicTypeKind type, bool is_array, int size, std::string name,
          Location loc)
      : DeclAST(DeclAST::VarDeclKind, std::move(name), loc), type(type),
        is_array(is_array), size(size) {}

  // Disable copy and move.
  VarDecl(const VarDecl &) = delete;
  VarDecl(VarDecl &&) = delete;
  VarDecl &operator=(const VarDecl &) = delete;
  VarDecl &operator=(VarDecl &&) = delete;

  BasicTypeKind getType() const { return type; }
  bool isArray() const { return is_array; }
  int getSize() const { return size; }

  static bool InstanceCheck(const DeclAST *x) {
    return x->getKind() == DeclAST::VarDeclKind;
  }
};

class ArgDecl;
class FuncDef : public DeclAST {
public:
  using StmtListType = StmtAST::StmtListType;

  FuncDef(BasicTypeKind return_type, std::vector<DeclAST *> args,
          std::vector<DeclAST *> decls, StmtListType stmts,
          std::string name, Location loc)
      : DeclAST(DeclAST::FuncDefKind, std::move(name), loc),
        return_type(return_type), args(std::move(args)),
        decls(std::move(decls)), stmts(std::move(stmts)) {}

  // Disable copy and move.
  FuncDef(const FuncDef &) = delete;
  FuncDef(FuncDef &&) = delete;
  FuncDef &operator=(const FuncDef &) = delete;
  FuncDef &operator=(FuncDef &&) = delete;

  BasicTypeKind getReturnType() const { return return_type; }
  void setReturnType(BasicTypeKind RetTy) { return_type = RetTy; }

  const std::vector<DeclAST *> &getArgs() const { return args; }
  std::vector<DeclAST *> &getArgs() { return args; }
  ArgDecl *getArgAt(unsigned Pos) const;
  size_t getNumArgs() const { return args.size(); }

  const std::vector<DeclAST *> &getDecls() const { return decls; }
  std::vector<DeclAST *> &getDecls() { return decls; }

  const StmtListType &getStmts() const { return stmts; }
  StmtListType &getStmts() { return stmts; }

  static bool InstanceCheck(const DeclAST *x) {
    return x->getKind() == DeclAST::FuncDefKind;
  }
private:
  friend class AST;
  BasicTypeKind return_type;
  std::vector<DeclAST *> args;
  std::vector<DeclAST *> decls;
  StmtListType stmts;
  ~FuncDef();
};

class ArgDecl : public DeclAST {
  friend class AST;
  BasicTypeKind type;
  ~ArgDecl() = default;

public:
  ArgDecl(BasicTypeKind type, std::string name, Location loc)
      : DeclAST(DeclAST::ArgDeclKind, std::move(name), loc), type(type) {}

  // Disable copy and move.
  ArgDecl(const ArgDecl &) = delete;
  ArgDecl(ArgDecl &&) = delete;
  ArgDecl &operator=(const ArgDecl &) = delete;
  ArgDecl &operator=(ArgDecl &&) = delete;

  BasicTypeKind getType() const { return type; }

  static bool InstanceCheck(const DeclAST *x) {
    return x->getKind() == DeclAST::ArgDeclKind;
  }
};

class ReadStmt : public StmtAST {
  friend class AST;
  std::vector<ExprAST *> names;
  ~ReadStmt();

public:
  ReadStmt(std::vector<ExprAST *> names, Location loc)
      : StmtAST(StmtAST::ReadStmtKind, loc), names(std::move(names)) {}

  // Disable copy and move.
  ReadStmt(const ReadStmt &) = delete;
  ReadStmt(ReadStmt &&) = delete;
  ReadStmt &operator=(const ReadStmt &) = delete;
  ReadStmt &operator=(ReadStmt &&) = delete;

  const std::vector<ExprAST *> &getNames() const { return names; }
  std::vector<ExprAST *> &getNames() { return names; }

  static bool InstanceCheck(const StmtAST *x) {
    return x->getKind() == StmtAST::ReadStmtKind;
  }
};

class WriteStmt : public StmtAST {
  friend class AST;
  ExprAST *str;
  ExprAST *value;
  ~WriteStmt();

public:
  WriteStmt(ExprAST *str, ExprAST *value, Location loc)
      : StmtAST(StmtAST::WriteStmtKind, loc), str(str), value(value) {}

  // Disable copy and move.
  WriteStmt(const WriteStmt &) = delete;
  WriteStmt(WriteStmt &&) = delete;
  WriteStmt &operator=(const WriteStmt &) = delete;
  WriteStmt &operator=(WriteStmt &&) = delete;

  ExprAST *getStr() const { return str; }
  ExprAST *getValue() const { return value; }
  void setValue(ExprAST *Val);

  static bool InstanceCheck(const StmtAST *x) {
    return x->getKind() == WriteStmtKind;
  }
};

class AssignStmt : public StmtAST {
  friend class AST;
  ExprAST *target;
  ExprAST *value;
  ~AssignStmt();

public:
  AssignStmt(ExprAST *target, ExprAST *value, Location loc)
      : StmtAST(StmtAST::AssignStmtKind, loc), target(target), value(value) {}

  // Disable copy and move.
  AssignStmt(const AssignStmt &) = delete;
  AssignStmt(AssignStmt &&) = delete;
  AssignStmt &operator=(const AssignStmt &) = delete;
  AssignStmt &operator=(AssignStmt &&) = delete;

  ExprAST *getTarget() const { return target; }
  ExprAST *getValue() const { return value; }
  void setValue(ExprAST *E);
  void setTarget(ExprAST *E);

  static bool InstanceCheck(const StmtAST *x) {
    return x->getKind() == AssignStmtKind;
  }
};

class ForStmt : public StmtAST {
  friend class AST;
  StmtAST *initial;
  ExprAST *condition;
  StmtAST *step;
  StmtListType body;
  ~ForStmt();

public:
  ForStmt(StmtAST *initial, ExprAST *condition, StmtAST *step,
          StmtListType body, Location loc)
      : StmtAST(StmtAST::ForStmtKind, loc), initial(initial),
        condition(condition), step(step), body(std::move(body)) {}

  // Disable copy and move.
  ForStmt(const ForStmt &) = delete;
  ForStmt(ForStmt &&) = delete;
  ForStmt &operator=(const ForStmt &) = delete;
  ForStmt &operator=(ForStmt &&) = delete;

  StmtAST *getInitial() const &{ return initial; }
  UniquePtrToAST getInitial() &&;

  ExprAST *getCondition() const &{ return condition; }
  void setCondition(ExprAST *E);
  UniquePtrToAST getCondition() &&;

  StmtAST *getStep() const &{ return step; }
  UniquePtrToAST getStep() &&;

  const StmtListType &getBody() const &{ return body; }
  StmtListType &getBody() &{ return body; }
  StmtListType getBody() &&{ return std::move(body); }

  static bool InstanceCheck(const StmtAST *x) {
    return x->getKind() == StmtAST::ForStmtKind;
  }
};

class WhileStmt : public StmtAST {
  friend class AST;
  ExprAST *condition;
  StmtListType body;
  ~WhileStmt();

public:
  WhileStmt(ExprAST *condition, StmtListType body,
            Location loc)
      : StmtAST(WhileStmtKind, loc), condition(condition),
        body(std::move(body)) {}

  // Disable copy and move.
  WhileStmt(const WhileStmt &) = delete;
  WhileStmt(WhileStmt &&) = delete;
  WhileStmt &operator=(const WhileStmt &) = delete;
  WhileStmt &operator=(WhileStmt &&) = delete;

  ExprAST *getCondition() const { return condition; }
  void setCondition(ExprAST *E);
  const StmtListType &getBody() const { return body; }
  StmtListType &getBody() { return body; }

  static bool InstanceCheck(const StmtAST *x) {
    return x->getKind() == WhileStmtKind;
  }
};

class ReturnStmt : public StmtAST {
  ExprAST *value;
  friend class AST;
  ~ReturnStmt();

public:
  ReturnStmt(ExprAST *value, Location loc)
      : StmtAST(StmtAST::ReturnStmtKind, loc), value(value) {}

  // Disable copy and move.
  ReturnStmt(const ReturnStmt &) = delete;
  ReturnStmt(ReturnStmt &&) = delete;
  ReturnStmt &operator=(const ReturnStmt &) = delete;
  ReturnStmt &operator=(ReturnStmt &&) = delete;

  ExprAST *getValue() const { return value; }
  void setValue(ExprAST *E);

  static bool InstanceCheck(const StmtAST *x) {
    return x->getKind() == StmtAST::ReturnStmtKind;
  }
};

class IfStmt : public StmtAST {
  ExprAST *test;
  StmtListType body;
  StmtListType orelse;
  friend class AST;
  ~IfStmt();

public:
  IfStmt(ExprAST *test, StmtListType body,
         StmtListType orelse, Location loc)
      : StmtAST(StmtAST::IfStmtKind, loc), test(test), body(std::move(body)),
        orelse(std::move(orelse)) {}

  // Disable copy and move.
  IfStmt(const IfStmt &) = delete;
  IfStmt(IfStmt &&) = delete;
  IfStmt &operator=(const IfStmt &) = delete;
  IfStmt &operator=(IfStmt &&) = delete;

  ExprAST *getTest() const { return test; }
  void setTest(ExprAST *E);
  const StmtListType &getBody() const &{ return body; }
  StmtListType &getBody() &{ return body; }
  StmtListType getBody() &&{ return std::move(body); }

  const StmtListType &getOrelse() const &{ return orelse; }
  StmtListType &getOrelse() &{ return orelse; }
  StmtListType getOrelse() &&{ return std::move(orelse); }

  static bool InstanceCheck(const StmtAST *x) {
    return x->getKind() == StmtAST::IfStmtKind;
  }
};

class ExprStmt : public StmtAST {
  ExprAST *value;
  friend class AST;
  ~ExprStmt();

public:
  ExprStmt(ExprAST *value, Location loc)
      : StmtAST(StmtAST::ExprStmtKind, loc), value(value) {}

  // Disable copy and move.
  ExprStmt(const ExprStmt &) = delete;
  ExprStmt(ExprStmt &&) = delete;
  ExprStmt &operator=(const ExprStmt &) = delete;
  ExprStmt &operator=(ExprStmt &&) = delete;

  ExprAST *getValue() const &{ return value; }
  UniquePtrToAST getValue() &&;
  void setValue(ExprAST *E);

  static bool InstanceCheck(const StmtAST *x) {
    return x->getKind() == StmtAST::ExprStmtKind;
  }
};

class BinOpExpr : public ExprAST {
  ExprAST *left;
  OperatorKind op;
  ExprAST *right;
  friend class AST;
  ~BinOpExpr();

public:
  BinOpExpr(ExprAST *left, OperatorKind op, ExprAST *right, Location loc)
      : ExprAST(ExprAST::BinOpExprKind, loc), left(left), op(op), right(right) {
  }

  // Disable copy and move.
  BinOpExpr(const BinOpExpr &) = delete;
  BinOpExpr(BinOpExpr &&) = delete;
  BinOpExpr &operator=(const BinOpExpr &) = delete;
  BinOpExpr &operator=(BinOpExpr &&) = delete;

  OperatorKind getOp() const { return op; }

  ExprAST *getLeft() const &{ return left; }
  UniquePtrToAST getLeft() &&;
  void setLeft(ExprAST *E);

  ExprAST *getRight() const &{ return right; }
  UniquePtrToAST getRight() &&;
  void setRight(ExprAST *E);

  static bool InstanceCheck(const ExprAST *x) {
    return x->getKind() == ExprAST::BinOpExprKind;
  }
};

class ParenExpr : public ExprAST {
  ExprAST *value;
  ~ParenExpr();
  friend class AST;
  friend class ExprAST;

  bool isConstantImpl() const { return value->isConstant(); }
  int getConstantValueImpl() const { return value->getConstantValue(); }

public:
  ParenExpr(ExprAST *value, Location loc)
      : ExprAST(ExprAST::ParenExprKind, loc), value(value) {}

  // Disable copy and move.
  ParenExpr(const ParenExpr &) = delete;
  ParenExpr(ParenExpr &&) = delete;
  ParenExpr &operator=(const ParenExpr &) = delete;
  ParenExpr &operator=(ParenExpr &&) = delete;

  ExprAST *getValue() const &{ return value; }
  UniquePtrToAST getValue() &&;
  void setValue(ExprAST *E);

  static bool InstanceCheck(const ExprAST *x) {
    return x->getKind() == ExprAST::ParenExprKind;
  }
};

class BoolOpExpr : public ExprAST {
  friend class AST;
  ExprAST *value;
  bool has_cmpop;
  void setHasCompareOp(bool B) { has_cmpop = B; }
  ~BoolOpExpr();

  friend class ExprAST;
  bool isConstantImpl() const { return value->isConstant(); }
  int getConstantValueImpl() const { return value->getConstantValue(); }

public:
  BoolOpExpr(ExprAST *value, bool has_cmpop, Location loc)
      : ExprAST(BoolOpExprKind, loc), value(value), has_cmpop(has_cmpop) {}

  // Disable copy and move.
  BoolOpExpr(const BoolOpExpr &) = delete;
  BoolOpExpr(BoolOpExpr &&) = delete;
  BoolOpExpr &operator=(const BoolOpExpr &) = delete;
  BoolOpExpr &operator=(BoolOpExpr &&) = delete;

  ExprAST *getValue() const &{ return value; }
  UniquePtrToAST getValue() &&;
  void setValue(ExprAST *E);
  bool hasCompareOp() const { return has_cmpop; }

  static bool InstanceCheck(const ExprAST *x) {
    return x->getKind() == BoolOpExprKind;
  }
  static bool isCompareOp(OperatorKind Op);
};

class UnaryOpExpr : public ExprAST {
  UnaryopKind op;
  ExprAST *operand;
  friend class AST;
  ~UnaryOpExpr();

  friend class ExprAST;
  bool isConstantImpl() const { return operand->isConstant(); }
  int getConstantValueImpl() const {
    auto OpVal = operand->getConstantValue();
    return op == UnaryopKind::USub ? -OpVal : OpVal;
  }

public:
  UnaryOpExpr(UnaryopKind op, ExprAST *operand, Location loc)
      : ExprAST(UnaryOpExprKind, loc), op(op), operand(operand) {}

  // Disable copy and move.
  UnaryOpExpr(const UnaryOpExpr &) = delete;
  UnaryOpExpr(UnaryOpExpr &&) = delete;
  UnaryOpExpr &operator=(const UnaryOpExpr &) = delete;
  UnaryOpExpr &operator=(UnaryOpExpr &&) = delete;

  UnaryopKind getOp() const { return op; }
  UniquePtrToAST getOperand() &&;
  ExprAST *getOperand() const &{ return operand; }
  void setOperand(ExprAST *E);

  static bool InstanceCheck(const ExprAST *x) {
    return x->getKind() == ExprAST::UnaryOpExprKind;
  }
};

class CallExpr : public ExprAST {
  std::string func;
  std::vector<ExprAST *> args;
  friend class AST;
  ~CallExpr();

public:
  CallExpr(std::string func, std::vector<ExprAST *> args, Location loc)
      : ExprAST(ExprAST::CallExprKind, loc), func(std::move(func)),
        args(std::move(args)) {}

  // Disable copy and move.
  CallExpr(const CallExpr &) = delete;
  CallExpr(CallExpr &&) = delete;
  CallExpr &operator=(const CallExpr &) = delete;
  CallExpr &operator=(CallExpr &&) = delete;

  const std::string &getFunc() const { return func; }
  const std::vector<ExprAST *> &getArgs() const { return args; }
  std::vector<ExprAST *> &getArgs() { return args; }
  ExprAST *getArgAt(unsigned I) const { return args[I]; }
  void setArgAt(unsigned I, ExprAST *Val);
  size_t getNumArgs() const { return args.size(); }

  static bool InstanceCheck(const ExprAST *x) {
    return x->getKind() == ExprAST::CallExprKind;
  }
};

class NumExpr : public ExprAST {
  int n;
  friend class AST;
  ~NumExpr() = default;

  friend class ExprAST;
  bool isConstantImpl() const { return true; }
  int getConstantValueImpl() const { return getN(); }

public:
  NumExpr(int n, Location loc)
      : ExprAST(ExprAST::NumExprKind, loc), n(n) {}

  // Disable copy and move.
  NumExpr(const NumExpr &) = delete;
  NumExpr(NumExpr &&) = delete;
  NumExpr &operator=(const NumExpr &) = delete;
  NumExpr &operator=(NumExpr &&) = delete;

  int getN() const { return n; }

  static bool InstanceCheck(const ExprAST *x) {
    return x->getKind() == ExprAST::NumExprKind;
  }
};

class StrExpr : public ExprAST {
  std::string s;
  friend class AST;
  ~StrExpr() = default;

public:
  StrExpr(std::string s, Location loc)
      : ExprAST(ExprAST::StrExprKind, loc), s(std::move(s)) {}

  // Disable copy and move.
  StrExpr(const StrExpr &) = delete;
  StrExpr(StrExpr &&) = delete;
  StrExpr &operator=(const StrExpr &) = delete;
  StrExpr &operator=(StrExpr &&) = delete;

  const std::string &getS() const { return s; }

  static bool InstanceCheck(const ExprAST *x) {
    return x->getKind() == ExprAST::StrExprKind;
  }
};

class CharExpr : public ExprAST {
  int c;
  friend class AST;
  ~CharExpr() = default;

  friend class ExprAST;
  bool isConstantImpl() const { return true; }
  int getConstantValueImpl() const { return getC(); }

public:
  CharExpr(int c, Location loc)
      : ExprAST(ExprAST::CharExprKind, loc), c(c) {}

  // Disable copy and move.
  CharExpr(const CharExpr &) = delete;
  CharExpr(CharExpr &&) = delete;
  CharExpr &operator=(const CharExpr &) = delete;
  CharExpr &operator=(CharExpr &&) = delete;

  int getC() const { return c; }

  static bool InstanceCheck(const ExprAST *x) {
    return x->getKind() == ExprAST::CharExprKind;
  }
};

class SubscriptExpr : public ExprAST {
  std::string name;
  ExprAST *index;
  ExprContextKind context;
  ~SubscriptExpr();
  friend class AST;

public:
  SubscriptExpr(std::string name, ExprAST *index, ExprContextKind ctx,
                Location loc)
      : ExprAST(SubscriptExprKind, loc), name(std::move(name)), index(index),
        context(ctx) {}

  // Disable copy and move.
  SubscriptExpr(const SubscriptExpr &) = delete;
  SubscriptExpr(SubscriptExpr &&) = delete;
  SubscriptExpr &operator=(const SubscriptExpr &) = delete;
  SubscriptExpr &operator=(SubscriptExpr &&) = delete;

  const std::string &getName() const { return name; }
  ExprContextKind getContext() const { return context; }
  ExprAST *getIndex() const &{ return index; }
  UniquePtrToAST getIndex() &&;
  void setIndex(ExprAST *E);

  static bool InstanceCheck(const ExprAST *x) {
    return x->getKind() == SubscriptExprKind;
  }
};

class NameExpr : public ExprAST {
  std::string id;
  ExprContextKind context;
  ~NameExpr() = default;
  friend class AST;

public:
  NameExpr(std::string id, ExprContextKind ctx, Location loc)
      : ExprAST(NameExprKind, loc), id(std::move(id)), context(ctx) {}

  // Disable copy and move.
  NameExpr(const NameExpr &) = delete;
  NameExpr(NameExpr &&) = delete;
  NameExpr &operator=(const NameExpr &) = delete;
  NameExpr &operator=(NameExpr &&) = delete;

  const std::string &getId() const { return id; }
  ExprContextKind getContext() const { return context; }

  static bool InstanceCheck(const ExprAST *x) {
    return x->getKind() == NameExprKind;
  }
};

class ProgramAST : public AST {
  std::string Filename;
  std::vector<DeclAST *> decls;
  friend class AST;
  ~ProgramAST();

public:
  ProgramAST(std::string Filename, std::vector<DeclAST *> decls)
      : AST(ProgramASTKind, Location()), Filename(std::move(Filename)),
        decls(std::move(decls)) {}

  // Disable copy and move.
  ProgramAST(const ProgramAST &) = delete;
  ProgramAST(ProgramAST &&) = delete;
  ProgramAST &operator=(const ProgramAST &) = delete;
  ProgramAST &operator=(ProgramAST &&) = delete;

  const std::vector<DeclAST *> &getDecls() const { return decls; }
  std::vector<DeclAST *> &getDecls() { return decls; }
  const std::string &getFilename() const { return Filename; }

  static bool InstanceCheck(const AST *A) {
    return A->getKind() == ProgramASTKind;
  }
};

/// Pretty print.
void PrettyPrintAST(const AST &A, std::ostream &O);
} // namespace simplecc
#endif