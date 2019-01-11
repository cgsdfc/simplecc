/// @file Definitions of the AST class hierarchy.
/// It is deliberate to have definitions for all AST classes in one place.
/// This is a header that gets included a lot. Be careful! Any modification
/// to this will cause compilation of half of the project.
#ifndef SIMPLECC_PARSE_AST_H
#define SIMPLECC_PARSE_AST_H
#include "simplecc/Support/Macros.h"
#include "simplecc/Lex/Location.h"
#include "simplecc/Parse/Enums.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// Forward declare all AST classes.
namespace simplecc {
#define HANDLE_AST(Class) class Class;
#include "simplecc/Parse/AST.def"
} // namespace simplecc

namespace simplecc {
/// Base class for all AST nodes.
/// The AST class hierarchy is designed to be a manually managed
/// polymorphism. Each concrete subclass is explicitly tagged.
/// Virtual functions are avoided.
/// The AST of simplecc is mostly for readonly traversal use cases.
/// Modifiers are supported in a limited and partial manner.
/// Nodes with expression children may support setters on those children,
/// such as AssignStmt and BinOpExpr.
/// Nodes with statement list may support rvalue-getter that transfers their statement
/// list to others.
/// Both of the above may also support rvalue-getter on the their expression children that
/// again transfers ownership.
/// AST nodes are created on the heap and referenced mostly via pointer (if not via reference). Each instance of
/// an AST owns their children unless they get stolen. Use deleteAST() to dispose them or
/// better, use the specialized version of ``std::unique_ptr`` -- UniquePtrToAST.
class AST {
  unsigned SubclassID;
  Location Loc;

  static const char *getClassName(unsigned Kind);
protected:
  /// Protected. Use concrete subclass constructor instead.
  AST(unsigned Kind, Location L) : SubclassID(Kind), Loc(L) {}
  /// Protected. Use deleteAST() instead.
  ~AST() = default;
public:
  enum ASTKind : unsigned {
#define HANDLE_AST(CLASS) CLASS##Kind,
#include "simplecc/Parse/AST.def"
  };

  /// Return the real class name.
  const char *getClassName() const { return getClassName(getKind()); }
  /// Return the subclass kind.
  unsigned getKind() const { return SubclassID; }
  /// Return the source file location where the node is created.
  Location getLocation() const { return Loc; }
  /// Delete an AST object. Replacement of ``delete``.
  void deleteAST();
  /// Print this AST with proper indentations.
  void Format(std::ostream &os) const;
  /// For debugging. Does the same as Format() but uses ``std::cerr``.
  void dump() const;
};

DEFINE_INLINE_OUTPUT_OPERATOR(AST)

/// This struct knows how to delete an AST or a sequence of AST.
/// Instead of calling deleteAST() directly, use methods of this struct.
/// The apply() overloads can handle various deletion use cases of AST.
struct DeleteAST {
  /// Delete a range of AST.
  template <typename Iterator>
  static void apply(Iterator first, Iterator last) {
    std::for_each(first, last, DeleteAST());
  }
  /// Delete a vector of AST, assuming the vector owns them.
  template <typename AstT> static void apply(const std::vector<AstT *> &List) {
    return apply(List.begin(), List.end());
  }
  /// Delete a single AST.
  static void apply(AST *A) {
    if (A)
      A->deleteAST();
  }
  /// Act as a functor. Replacement of ``std::default_delete``.
  void operator()(AST *A) const { return apply(A); }
};

/// A specialized version of ``std::unique_ptr`` that works with AST.
using UniquePtrToAST = std::unique_ptr<AST, DeleteAST>;

/// This is the base class for all declaration nodes.
class DeclAST : public AST {
  /// Declaration always has a type.
  BasicTypeKind Type;
  /// Declaration always has a name.
  std::string Name;

protected:
  /// Protected, use subclass constructors.
  DeclAST(unsigned int Kind, BasicTypeKind Ty, std::string name, Location loc)
      : AST(Kind, loc), Type(Ty), Name(std::move(name)) {}
  /// Return the type of this declaration.
  BasicTypeKind getType() const { return Type; }
  void setType(BasicTypeKind Ty) { Type = Ty; }
public:
  /// Return the name of this declaration.
  const std::string &getName() const &{ return Name; }
  static bool InstanceCheck(const AST *A);
};

/// This is the base class for all statement nodes.
class StmtAST : public AST {
protected:
  /// Protected, use subclass constructors.
  StmtAST(unsigned int Kind, Location loc) : AST(Kind, loc) {}

public:
  /// The type that holds a list of StmtAST nodes.
  using StmtListType = std::vector<StmtAST *>;
  static bool InstanceCheck(const AST *A);
};

/// This is the base class for all expression nodes.
class ExprAST : public AST {
protected:
  /// Protected, use subclass constructors.
  ExprAST(unsigned Kind, Location loc) : AST(Kind, loc) {}
  /// Default impl for isConstant().
  /// Subclass that cannot determine constantness falls back to this.
  bool isConstantImpl() const { return false; }

  /// Default impl for getConstantValue().
  /// Subclass that cannot determine constantness always returns false
  /// in isConstant() so getConstant() should never be called on it.
  /// Assert on that case.
  int getConstantValueImpl() const {
    assert(false && "subclass should implement this!");
    return 0; // for MSVC to be happy.
  }

public:
  /// Return true if this node is mostly a literal value and can be evaluated to int
  /// What is mostly a literal value?
  /// 1. CharExpr or NumExpr.
  /// 2. BoolOpExpr, UnaryOpExpr or ParenOpExpr that wraps 1. in their value or operand field.
  /// 3. BinOpExpr is **not** constant.
  bool isConstant() const;

  /// Return the constant value if isConstant() returns true.
  int getConstantValue() const;

  /// Return true if this is constant zero.
  bool isZeroVal() const { return isConstant() && 0 == getConstantValue(); }

  /// Return true if this is constant one.
  bool isOneVal() const { return isConstant() && 1 == getConstantValue(); }

  static bool InstanceCheck(const AST *A);
};
// TODO: use UniquePtrToAST and vector<AST*> as data member.
// save a lot of destructor, getter and setter code.
// more safe when OOM.

/// This class represents a constant declaration.
class ConstDecl : public DeclAST {
  friend class AST;
  /// The value associated with this ConstDecl. It must be either CharExpr or NumExpr.
  ExprAST *value;
  ~ConstDecl() { DeleteAST::apply(value); }

public:
  ConstDecl(BasicTypeKind type, std::string name, ExprAST *value, Location loc)
      : DeclAST(DeclAST::ConstDeclKind, type, std::move(name), loc), value(value) {}

  // Disable copy and move.
  ConstDecl(const ConstDecl &) = delete;
  ConstDecl(ConstDecl &&) = delete;
  ConstDecl &operator=(const ConstDecl &) = delete;
  ConstDecl &operator=(ConstDecl &&) = delete;

  /// Return the type of this ConstDecl.
  using DeclAST::getType;
  /// Return the constant value of the ConstDecl.
  ExprAST *getValue() const { return value; }

  static bool InstanceCheck(const DeclAST *x) {
    return x->getKind() == DeclAST::ConstDeclKind;
  }
};

/// This class represents a variable declaration.
class VarDecl : public DeclAST {
  friend class AST;
  /// Whether this declares an array.
  bool IsArray;
  /// If IsArray is true, this is the size of the array.
  /// If it is not an array, the value is 0.
  /// Note: even if IsArray is true, this can be a non-positive value
  /// since the Parser permits that.
  int size;
  ~VarDecl() = default;

public:
  VarDecl(BasicTypeKind Type, std::string Name, bool isArray, int size, Location loc)
      : DeclAST(DeclAST::VarDeclKind, Type, std::move(Name), loc), IsArray(isArray), size(size) {}

  // Disable copy and move.
  VarDecl(const VarDecl &) = delete;
  VarDecl(VarDecl &&) = delete;
  VarDecl &operator=(const VarDecl &) = delete;
  VarDecl &operator=(VarDecl &&) = delete;

  /// Return the type of this VarDecl.
  using DeclAST::getType;
  /// Return if this declares an array.
  bool isArray() const { return IsArray; }

  /// Return the size of the array.
  int getSize() const { return size; }

  static bool InstanceCheck(const DeclAST *x) {
    return x->getKind() == DeclAST::VarDeclKind;
  }
};

/// This class represents a function definition.
/// There is no function declaration or prototype in this language.
class FuncDef : public DeclAST {
public:
  /// Forward from StmtAST.
  using StmtListType = StmtAST::StmtListType;

  FuncDef(BasicTypeKind return_type, std::vector<ArgDecl *> args,
          std::vector<DeclAST *> decls, StmtListType stmts,
          std::string name, Location loc)
      : DeclAST(DeclAST::FuncDefKind, return_type, std::move(name), loc),
        Args(std::move(args)), Decls(std::move(decls)), Stmts(std::move(stmts)) {}

  // Disable copy and move.
  FuncDef(const FuncDef &) = delete;
  FuncDef(FuncDef &&) = delete;
  FuncDef &operator=(const FuncDef &) = delete;
  FuncDef &operator=(FuncDef &&) = delete;

  /// Return the return type of this function.
  BasicTypeKind getReturnType() const { return getType(); }

  /// This **changes** the return type of the function.
  /// Be careful! This will invalidate the result of type checking
  /// and break any consistency between return-stmt and return type.
  void setReturnType(BasicTypeKind RetTy) { setType(RetTy); }

  /// Return the formal argument list.
  const std::vector<ArgDecl *> &getArgs() const { return Args; }
  std::vector<ArgDecl *> &getArgs() { return Args; }

  /// Return the argument at specific position.
  /// Index starts from 0.
  ArgDecl *getArgAt(unsigned Pos) const;
  /// Return the number of formal arguments.
  size_t getNumArgs() const { return Args.size(); }

  /// Return the list of local declarations.
  const std::vector<DeclAST *> &getDecls() const { return Decls; }
  std::vector<DeclAST *> &getDecls() { return Decls; }

  /// Return the list of statement (function body).
  const StmtListType &getStmts() const { return Stmts; }
  StmtListType &getStmts() { return Stmts; }

  static bool InstanceCheck(const DeclAST *x) {
    return x->getKind() == DeclAST::FuncDefKind;
  }
private:
  friend class AST;
  std::vector<ArgDecl *> Args;
  std::vector<DeclAST *> Decls;
  StmtListType Stmts;
  ~FuncDef();
};

/// This class represents a formal argument in the function declaration.
class ArgDecl : public DeclAST {
  friend class AST;
  ~ArgDecl() = default;

public:
  ArgDecl(BasicTypeKind type, std::string name, Location loc)
      : DeclAST(DeclAST::ArgDeclKind, type, std::move(name), loc) {}

  // Disable copy and move.
  ArgDecl(const ArgDecl &) = delete;
  ArgDecl(ArgDecl &&) = delete;
  ArgDecl &operator=(const ArgDecl &) = delete;
  ArgDecl &operator=(ArgDecl &&) = delete;

  /// Return the type of this ArgDecl.
  using DeclAST::getType;

  static bool InstanceCheck(const DeclAST *x) {
    return x->getKind() == DeclAST::ArgDeclKind;
  }
};

/// This class represents a read statement, or a call to the scanf() builtin.
class ReadStmt : public StmtAST {
  friend class AST;
  std::vector<ExprAST *> Names;
  ~ReadStmt();

public:
  ReadStmt(std::vector<ExprAST *> names, Location loc)
      : StmtAST(StmtAST::ReadStmtKind, loc), Names(std::move(names)) {}

  // Disable copy and move.
  ReadStmt(const ReadStmt &) = delete;
  ReadStmt(ReadStmt &&) = delete;
  ReadStmt &operator=(const ReadStmt &) = delete;
  ReadStmt &operator=(ReadStmt &&) = delete;

  /// Return the list of names in the scanf().
  const std::vector<ExprAST *> &getNames() const { return Names; }
  std::vector<ExprAST *> &getNames() { return Names; }

  static bool InstanceCheck(const StmtAST *x) {
    return x->getKind() == StmtAST::ReadStmtKind;
  }
};

/// This class represents a write statement, or a call to the printf() builtin.
class WriteStmt : public StmtAST {
  friend class AST;
  ExprAST *Str;
  ExprAST *Value;
  ~WriteStmt();

public:
  WriteStmt(ExprAST *str, ExprAST *value, Location loc)
      : StmtAST(StmtAST::WriteStmtKind, loc), Str(str), Value(value) {}

  // Disable copy and move.
  WriteStmt(const WriteStmt &) = delete;
  WriteStmt(WriteStmt &&) = delete;
  WriteStmt &operator=(const WriteStmt &) = delete;
  WriteStmt &operator=(WriteStmt &&) = delete;

  /// Return the string literal if any.
  ExprAST *getStr() const { return Str; }
  /// Return the expression value if any.
  ExprAST *getValue() const { return Value; }
  /// Set expression value.
  void setValue(ExprAST *Val);

  static bool InstanceCheck(const StmtAST *x) {
    return x->getKind() == WriteStmtKind;
  }
};

/// This class represents an assign statement.
class AssignStmt : public StmtAST {
  friend class AST;
  ExprAST *Target;
  ExprAST *Value;
  ~AssignStmt();

public:
  AssignStmt(ExprAST *target, ExprAST *value, Location loc)
      : StmtAST(StmtAST::AssignStmtKind, loc), Target(target), Value(value) {}

  // Disable copy and move.
  AssignStmt(const AssignStmt &) = delete;
  AssignStmt(AssignStmt &&) = delete;
  AssignStmt &operator=(const AssignStmt &) = delete;
  AssignStmt &operator=(AssignStmt &&) = delete;

  /// Return the LHS.
  ExprAST *getTarget() const { return Target; }
  /// Return the RHS.
  ExprAST *getValue() const { return Value; }

  /// Set the LHS.
  void setTarget(ExprAST *E);
  /// Set the RHS.
  void setValue(ExprAST *E);

  static bool InstanceCheck(const StmtAST *x) {
    return x->getKind() == AssignStmtKind;
  }
};

/// This class represents a for statement.
class ForStmt : public StmtAST {
  friend class AST;
  StmtAST *Initial;
  ExprAST *Cond;
  StmtAST *Step;
  StmtListType Body;
  ~ForStmt();

public:
  ForStmt(StmtAST *initial, ExprAST *condition, StmtAST *step,
          StmtListType body, Location loc)
      : StmtAST(StmtAST::ForStmtKind, loc), Initial(initial),
        Cond(condition), Step(step), Body(std::move(body)) {}

  // Disable copy and move.
  ForStmt(const ForStmt &) = delete;
  ForStmt(ForStmt &&) = delete;
  ForStmt &operator=(const ForStmt &) = delete;
  ForStmt &operator=(ForStmt &&) = delete;

  /// Return the initial statement.
  StmtAST *getInitial() const &{ return Initial; }
  UniquePtrToAST getInitial() &&;

  /// Return the condition expression.
  ExprAST *getCondition() const &{ return Cond; }
  void setCondition(ExprAST *E);
  UniquePtrToAST getCondition() &&;

  /// Return the step statement.
  StmtAST *getStep() const &{ return Step; }
  UniquePtrToAST getStep() &&;

  /// Return the body.
  const StmtListType &getBody() const &{ return Body; }
  StmtListType &getBody() &{ return Body; }
  StmtListType getBody() &&{ return std::move(Body); }

  static bool InstanceCheck(const StmtAST *x) {
    return x->getKind() == StmtAST::ForStmtKind;
  }
};

/// This class represents a while statement.
class WhileStmt : public StmtAST {
  friend class AST;
  ExprAST *Cond;
  StmtListType Body;
  ~WhileStmt();

public:
  WhileStmt(ExprAST *condition, StmtListType body,
            Location loc)
      : StmtAST(WhileStmtKind, loc), Cond(condition),
        Body(std::move(body)) {}

  // Disable copy and move.
  WhileStmt(const WhileStmt &) = delete;
  WhileStmt(WhileStmt &&) = delete;
  WhileStmt &operator=(const WhileStmt &) = delete;
  WhileStmt &operator=(WhileStmt &&) = delete;

  /// Return the condition expression.
  ExprAST *getCondition() const { return Cond; }
  /// Set the condition.
  void setCondition(ExprAST *E);

  /// Return the body.
  const StmtListType &getBody() const { return Body; }
  StmtListType &getBody() { return Body; }

  static bool InstanceCheck(const StmtAST *x) {
    return x->getKind() == WhileStmtKind;
  }
};

/// This class represents a return statement.
class ReturnStmt : public StmtAST {
  /// This is an optional field.
  ExprAST *Value;
  friend class AST;
  ~ReturnStmt();

public:
  ReturnStmt(ExprAST *value, Location loc)
      : StmtAST(StmtAST::ReturnStmtKind, loc), Value(value) {}

  // Disable copy and move.
  ReturnStmt(const ReturnStmt &) = delete;
  ReturnStmt(ReturnStmt &&) = delete;
  ReturnStmt &operator=(const ReturnStmt &) = delete;
  ReturnStmt &operator=(ReturnStmt &&) = delete;

  bool hasValue() const { return !!Value; }
  ExprAST *getValue() const { return Value; }
  void setValue(ExprAST *E);

  static bool InstanceCheck(const StmtAST *x) {
    return x->getKind() == StmtAST::ReturnStmtKind;
  }
};

/// This class represents an if statement.
class IfStmt : public StmtAST {
  ExprAST *Cond;
  StmtListType Then;
  StmtListType Else;
  friend class AST;
  ~IfStmt();

public:
  IfStmt(ExprAST *C, StmtListType T, StmtListType E, Location loc)
      : StmtAST(StmtAST::IfStmtKind, loc), Cond(C), Then(std::move(T)),
        Else(std::move(E)) {}

  // Disable copy and move.
  IfStmt(const IfStmt &) = delete;
  IfStmt(IfStmt &&) = delete;
  IfStmt &operator=(const IfStmt &) = delete;
  IfStmt &operator=(IfStmt &&) = delete;

  /// Return the condition.
  ExprAST *getCondition() const { return Cond; }
  void setCondition(ExprAST *E);

  const StmtListType &getThen() const &{ return Then; }
  StmtListType &getThen() &{ return Then; }
  StmtListType getThen() &&{ return std::move(Then); }

  const StmtListType &getElse() const &{ return Else; }
  StmtListType &getElse() &{ return Else; }
  StmtListType getElse() &&{ return std::move(Else); }

  static bool InstanceCheck(const StmtAST *x) {
    return x->getKind() == StmtAST::IfStmtKind;
  }
};

/// This class represents an expression statement, which is effectively
/// a call expression.
class ExprStmt : public StmtAST {
  ExprAST *Value;
  friend class AST;
  ~ExprStmt();

public:
  ExprStmt(ExprAST *value, Location loc)
      : StmtAST(StmtAST::ExprStmtKind, loc), Value(value) {}

  // Disable copy and move.
  ExprStmt(const ExprStmt &) = delete;
  ExprStmt(ExprStmt &&) = delete;
  ExprStmt &operator=(const ExprStmt &) = delete;
  ExprStmt &operator=(ExprStmt &&) = delete;

  ExprAST *getValue() const &{ return Value; }
  UniquePtrToAST getValue() &&;
  void setValue(ExprAST *E);

  static bool InstanceCheck(const StmtAST *x) {
    return x->getKind() == StmtAST::ExprStmtKind;
  }
};

/// This class represents an binary operator expression.
class BinOpExpr : public ExprAST {
  ExprAST *Left;
  BinaryOpKind Op;
  ExprAST *Right;
  friend class AST;
  ~BinOpExpr();

public:
  BinOpExpr(ExprAST *left, BinaryOpKind op, ExprAST *right, Location loc)
      : ExprAST(ExprAST::BinOpExprKind, loc), Left(left), Op(op), Right(right) {
  }

  // Disable copy and move.
  BinOpExpr(const BinOpExpr &) = delete;
  BinOpExpr(BinOpExpr &&) = delete;
  BinOpExpr &operator=(const BinOpExpr &) = delete;
  BinOpExpr &operator=(BinOpExpr &&) = delete;

  BinaryOpKind getOp() const { return Op; }

  ExprAST *getLeft() const &{ return Left; }
  UniquePtrToAST getLeft() &&;
  void setLeft(ExprAST *E);

  ExprAST *getRight() const &{ return Right; }
  UniquePtrToAST getRight() &&;
  void setRight(ExprAST *E);

  static bool InstanceCheck(const ExprAST *x) {
    return x->getKind() == ExprAST::BinOpExprKind;
  }
};

/// This class represents an expression in parentheses.
class ParenExpr : public ExprAST {
  ExprAST *Value;
  ~ParenExpr();
  friend class AST;
  friend class ExprAST;

  bool isConstantImpl() const { return Value->isConstant(); }
  int getConstantValueImpl() const { return Value->getConstantValue(); }

public:
  ParenExpr(ExprAST *value, Location loc)
      : ExprAST(ExprAST::ParenExprKind, loc), Value(value) {}

  // Disable copy and move.
  ParenExpr(const ParenExpr &) = delete;
  ParenExpr(ParenExpr &&) = delete;
  ParenExpr &operator=(const ParenExpr &) = delete;
  ParenExpr &operator=(ParenExpr &&) = delete;

  ExprAST *getValue() const &{ return Value; }
  UniquePtrToAST getValue() &&;
  void setValue(ExprAST *E);

  static bool InstanceCheck(const ExprAST *x) {
    return x->getKind() == ExprAST::ParenExprKind;
  }
};

/// This class represents a boolean operator expression.
class BoolOpExpr : public ExprAST {
  friend class AST;
  ExprAST *Value;
  bool HasCmpOp;
  void setHasCompareOp(bool B) { HasCmpOp = B; }
  ~BoolOpExpr();

  friend class ExprAST;
  bool isConstantImpl() const { return Value->isConstant(); }
  int getConstantValueImpl() const { return Value->getConstantValue(); }
  /// Impl of hasCompareOp().
  static bool isCompareOp(BinaryOpKind Op);

public:
  BoolOpExpr(ExprAST *value, bool has_cmpop, Location loc)
      : ExprAST(BoolOpExprKind, loc), Value(value), HasCmpOp(has_cmpop) {}

  // Disable copy and move.
  BoolOpExpr(const BoolOpExpr &) = delete;
  BoolOpExpr(BoolOpExpr &&) = delete;
  BoolOpExpr &operator=(const BoolOpExpr &) = delete;
  BoolOpExpr &operator=(BoolOpExpr &&) = delete;

  // TODO: Don't wrap any node, be itself.

  /// Return the wrapped node.
  ExprAST *getValue() const &{ return Value; }
  UniquePtrToAST getValue() &&;
  /// Set the wrapped node.
  void setValue(ExprAST *E);

  // TODO: Let CompareOp be themselves.

  /// Return if the wrapped node has a compare operator.
  /// a compare operator is also known as a rich compare operator.
  /// They are the six of the BinaryOpKind enum -- Eq, NotEq, Lt, LtE, Gt, GtE.
  bool hasCompareOp() const { return HasCmpOp; }

  static bool InstanceCheck(const ExprAST *x) {
    return x->getKind() == BoolOpExprKind;
  }
};

/// This class represents a unary operator expression.
class UnaryOpExpr : public ExprAST {
  UnaryOpKind Op;
  ExprAST *Operand;
  friend class AST;
  ~UnaryOpExpr();

  /// For ExprAST to able to call.
  friend class ExprAST;
  bool isConstantImpl() const { return Operand->isConstant(); }
  // TODO: put evaluation of constant expression in its own class just like pretty-printing.
  // so that the logic can be reused and don't just hidden in TrivialConstantFolder.
  /// Evaluate it on the fly.
  int getConstantValueImpl() const {
    auto OpVal = Operand->getConstantValue();
    return Op == UnaryOpKind::USub ? -OpVal : OpVal;
  }

public:
  UnaryOpExpr(UnaryOpKind op, ExprAST *operand, Location loc)
      : ExprAST(UnaryOpExprKind, loc), Op(op), Operand(operand) {}

  // Disable copy and move.
  UnaryOpExpr(const UnaryOpExpr &) = delete;
  UnaryOpExpr(UnaryOpExpr &&) = delete;
  UnaryOpExpr &operator=(const UnaryOpExpr &) = delete;
  UnaryOpExpr &operator=(UnaryOpExpr &&) = delete;

  /// Return the unary operator.
  UnaryOpKind getOp() const { return Op; }
  /// Return the operand.
  UniquePtrToAST getOperand() &&;
  ExprAST *getOperand() const &{ return Operand; }
  /// Set the operand.
  void setOperand(ExprAST *E);

  static bool InstanceCheck(const ExprAST *x) {
    return x->getKind() == ExprAST::UnaryOpExprKind;
  }
};

/// This class represents a call expression.
class CallExpr : public ExprAST {
  std::string Callee;
  std::vector<ExprAST *> Args;
  friend class AST;
  ~CallExpr();

public:
  CallExpr(std::string func, std::vector<ExprAST *> args, Location loc)
      : ExprAST(ExprAST::CallExprKind, loc),
        Callee(std::move(func)), Args(std::move(args)) {}

  // Disable copy and move.
  CallExpr(const CallExpr &) = delete;
  CallExpr(CallExpr &&) = delete;
  CallExpr &operator=(const CallExpr &) = delete;
  CallExpr &operator=(CallExpr &&) = delete;

  /// Return the name of function being called.
  const std::string &getCallee() const { return Callee; }
  /// Return the actual arguments passed to the function.
  const std::vector<ExprAST *> &getArgs() const { return Args; }
  std::vector<ExprAST *> &getArgs() { return Args; }

  /// Return the actual argument at specific position.
  ExprAST *getArgAt(unsigned I) const { return Args[I]; }
  /// Set the actual argument at specific position.
  void setArgAt(unsigned I, ExprAST *Val);

  /// Return the number of actual arguments.
  size_t getNumArgs() const { return Args.size(); }

  static bool InstanceCheck(const ExprAST *x) {
    return x->getKind() == ExprAST::CallExprKind;
  }
};

/// This class represent a number expression, which is effectively an unsigned
/// integer literal.
class NumExpr : public ExprAST {
  int TheNum;
  friend class AST;
  ~NumExpr() = default;

  friend class ExprAST;
  bool isConstantImpl() const { return true; }
  int getConstantValueImpl() const { return getNum(); }

public:
  NumExpr(int n, Location loc)
      : ExprAST(ExprAST::NumExprKind, loc), TheNum(n) {}

  // Disable copy and move.
  NumExpr(const NumExpr &) = delete;
  NumExpr(NumExpr &&) = delete;
  NumExpr &operator=(const NumExpr &) = delete;
  NumExpr &operator=(NumExpr &&) = delete;

  /// Return the value of the integer literal.
  int getNum() const { return TheNum; }

  static bool InstanceCheck(const ExprAST *x) {
    return x->getKind() == ExprAST::NumExprKind;
  }
};

/// This class represents a string literal expression.
class StrExpr : public ExprAST {
  std::string TheStr;
  friend class AST;
  ~StrExpr() = default;

public:
  StrExpr(std::string s, Location loc)
      : ExprAST(ExprAST::StrExprKind, loc), TheStr(std::move(s)) {}

  // Disable copy and move.
  StrExpr(const StrExpr &) = delete;
  StrExpr(StrExpr &&) = delete;
  StrExpr &operator=(const StrExpr &) = delete;
  StrExpr &operator=(StrExpr &&) = delete;

  /// Return the value of the string literal.
  const std::string &getStr() const { return TheStr; }

  static bool InstanceCheck(const ExprAST *x) {
    return x->getKind() == ExprAST::StrExprKind;
  }
};

/// This class represents a character literal expression.
class CharExpr : public ExprAST {
  // TODO: Should we store it as a char? How to handle type mapping
  // from source lang to impl lang and then to IR?
  int TheChar;
  // TypeMap::Char TheChar;
  // struct TypeMap {
  //   using Char = char;
  friend class AST;
  ~CharExpr() = default;

  friend class ExprAST;
  bool isConstantImpl() const { return true; }
  int getConstantValueImpl() const { return getChar(); }

public:
  CharExpr(int c, Location loc)
      : ExprAST(ExprAST::CharExprKind, loc), TheChar(c) {}

  // Disable copy and move.
  CharExpr(const CharExpr &) = delete;
  CharExpr(CharExpr &&) = delete;
  CharExpr &operator=(const CharExpr &) = delete;
  CharExpr &operator=(CharExpr &&) = delete;

  /// Return the character value.
  int getChar() const { return TheChar; }

  static bool InstanceCheck(const ExprAST *x) {
    return x->getKind() == ExprAST::CharExprKind;
  }
};

/// This class represents a subscript expression.
class SubscriptExpr : public ExprAST {
  std::string ArrayName;
  ExprAST *Index;
  ExprContextKind context;
  ~SubscriptExpr();
  friend class AST;

public:
  SubscriptExpr(std::string name, ExprAST *index, ExprContextKind ctx,
                Location loc)
      : ExprAST(SubscriptExprKind, loc), ArrayName(std::move(name)), Index(index),
        context(ctx) {}

  // Disable copy and move.
  SubscriptExpr(const SubscriptExpr &) = delete;
  SubscriptExpr(SubscriptExpr &&) = delete;
  SubscriptExpr &operator=(const SubscriptExpr &) = delete;
  SubscriptExpr &operator=(SubscriptExpr &&) = delete;

  /// Return the name of the array.
  const std::string &getArrayName() const { return ArrayName; }
  /// Return the expression context.
  ExprContextKind getContext() const { return context; }
  /// Return the index expression.
  ExprAST *getIndex() const &{ return Index; }
  UniquePtrToAST getIndex() &&;
  /// Set the index expression.
  void setIndex(ExprAST *E);

  // TODO: use classof() and borrow llvm/Support/Casting.h
  static bool InstanceCheck(const ExprAST *x) {
    return x->getKind() == SubscriptExprKind;
  }
};

/// This class represents a name expression, which is effectively an identifier.
class NameExpr : public ExprAST {
  std::string TheName;
  ExprContextKind context;
  ~NameExpr() = default;
  friend class AST;

public:
  NameExpr(std::string id, ExprContextKind ctx, Location loc)
      : ExprAST(NameExprKind, loc), TheName(std::move(id)), context(ctx) {}

  // Disable copy and move.
  NameExpr(const NameExpr &) = delete;
  NameExpr(NameExpr &&) = delete;
  NameExpr &operator=(const NameExpr &) = delete;
  NameExpr &operator=(NameExpr &&) = delete;

  /// Return the value of this name.
  const std::string &getName() const { return TheName; }
  /// Return the expression context of this name.
  ExprContextKind getContext() const { return context; }

  static bool InstanceCheck(const ExprAST *x) {
    return x->getKind() == NameExprKind;
  }
};

/// This class represents the whole program as the top level AST node.
class ProgramAST : public AST {
  std::string Filename;
  std::vector<DeclAST *> Decls;
  friend class AST;
  ~ProgramAST();

public:
  ProgramAST(std::string Filename, std::vector<DeclAST *> decls)
      : AST(ProgramASTKind, Location()), Filename(std::move(Filename)),
        Decls(std::move(decls)) {}

  // TODO: allow move, only disable copy.
  // Disable copy and move.
  ProgramAST(const ProgramAST &) = delete;
  ProgramAST(ProgramAST &&) = delete;
  ProgramAST &operator=(const ProgramAST &) = delete;
  ProgramAST &operator=(ProgramAST &&) = delete;

  /// Return the declaration list.
  const std::vector<DeclAST *> &getDecls() const { return Decls; }
  std::vector<DeclAST *> &getDecls() { return Decls; }

  /// Return the name of the file that produced this AST.
  const std::string &getFilename() const { return Filename; }

  static bool InstanceCheck(const AST *A) {
    return A->getKind() == ProgramASTKind;
  }
};

/// Pretty print.
void PrettyPrintAST(const AST &A, std::ostream &O);
} // namespace simplecc
#endif