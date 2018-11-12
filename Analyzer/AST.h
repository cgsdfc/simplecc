#ifndef AST_H
#define AST_H

#include "tokenize.h"

#include <vector>
#include <iostream>
#include <optional>

class AST {
public:
    virtual void Format(std::ostream &os) const = 0;
    virtual ~AST() = 0;
};

inline AST::~AST() {}

inline std::ostream &operator<<(std::ostream &os, const AST *ast) {
    ast->Format(os);
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const AST &ast) {
    return os << &ast;
}

template<class T>
inline std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
    os << "[";
    for (auto b = v.begin(), e = v.end(); b != e; ++b) {
        os << *b;
        if (b != e - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

class Program;

class Decl;

class ConstDecl;

class VarDecl;

class FuncDef;

struct Arg;

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

enum class OperatorKind {Add, Sub, Mult, Div, Eq, NotEq, Lt, LtE, Gt, GtE};
inline std::ostream &operator<<(std::ostream &os, OperatorKind val) {
    switch (val) {
    case OperatorKind::Add: return os << "OperatorKind::Add";
    case OperatorKind::Sub: return os << "OperatorKind::Sub";
    case OperatorKind::Mult: return os << "OperatorKind::Mult";
    case OperatorKind::Div: return os << "OperatorKind::Div";
    case OperatorKind::Eq: return os << "OperatorKind::Eq";
    case OperatorKind::NotEq: return os << "OperatorKind::NotEq";
    case OperatorKind::Lt: return os << "OperatorKind::Lt";
    case OperatorKind::LtE: return os << "OperatorKind::LtE";
    case OperatorKind::Gt: return os << "OperatorKind::Gt";
    case OperatorKind::GtE: return os << "OperatorKind::GtE";
    }
}


enum class UnaryopKind {UAdd, USub};
inline std::ostream &operator<<(std::ostream &os, UnaryopKind val) {
    switch (val) {
    case UnaryopKind::UAdd: return os << "UnaryopKind::UAdd";
    case UnaryopKind::USub: return os << "UnaryopKind::USub";
    }
}


enum class ExprContextKind {Load, Store};
inline std::ostream &operator<<(std::ostream &os, ExprContextKind val) {
    switch (val) {
    case ExprContextKind::Load: return os << "ExprContextKind::Load";
    case ExprContextKind::Store: return os << "ExprContextKind::Store";
    }
}


enum class BasicTypeKind {Int, Character, Void};
inline std::ostream &operator<<(std::ostream &os, BasicTypeKind val) {
    switch (val) {
    case BasicTypeKind::Int: return os << "BasicTypeKind::Int";
    case BasicTypeKind::Character: return os << "BasicTypeKind::Character";
    case BasicTypeKind::Void: return os << "BasicTypeKind::Void";
    }
}


class VarType;


class Program: public AST {
public:
    std::vector<Decl*> decls;
    Program(const std::vector<Decl*>& decls): decls(decls) {}
    ~Program() override;
    void Format(std::ostream &os) const override;
};

class Decl: public AST {
public:
    enum {ConstDecl, VarDecl, FuncDef};
    virtual int SubclassKind() const = 0;
};

class ConstDecl: public Decl {
public:
    BasicTypeKind type;
    std::string name;
    Expr* value;
    Location loc;
    ConstDecl(BasicTypeKind type, const std::string& name, Expr* value, const
              Location& loc): type(type), name(name), value(value), loc(loc) {}
    ~ConstDecl() override;
    void Format(std::ostream &os) const override;
    int SubclassKind() const override {
        return Decl::ConstDecl;
    }
};

class VarDecl: public Decl {
public:
    VarType* type;
    std::string name;
    Location loc;
    VarDecl(VarType* type, const std::string& name, const Location& loc):
            type(type), name(name), loc(loc) {}
    ~VarDecl() override;
    void Format(std::ostream &os) const override;
    int SubclassKind() const override {
        return Decl::VarDecl;
    }
};

class FuncDef: public Decl {
public:
    BasicTypeKind return_type;
    std::string name;
    std::vector<Arg*> args;
    std::vector<Decl*> decls;
    std::vector<Stmt*> stmts;
    Location loc;
    FuncDef(BasicTypeKind return_type, const std::string& name, const
            std::vector<Arg*>& args, const std::vector<Decl*>& decls, const
            std::vector<Stmt*>& stmts, const Location& loc):
            return_type(return_type), name(name), args(args), decls(decls),
            stmts(stmts), loc(loc) {}
    ~FuncDef() override;
    void Format(std::ostream &os) const override;
    int SubclassKind() const override {
        return Decl::FuncDef;
    }
};

struct Arg: public AST {
    BasicTypeKind type;
    std::string name;
    Location loc;
    Arg(BasicTypeKind type, const std::string& name, const Location& loc):
        type(type), name(name), loc(loc) {}
    ~Arg() override;
    void Format(std::ostream &os) const override;
};

class Stmt: public AST {
public:
    enum {Read, Write, Assign, For, While, Return, If, ExprStmt};
    virtual int SubclassKind() const = 0;
};

class Read: public Stmt {
public:
    std::vector<std::string> names;
    Location loc;
    Read(const std::vector<std::string>& names, const Location& loc):
         names(names), loc(loc) {}
    ~Read() override;
    void Format(std::ostream &os) const override;
    int SubclassKind() const override {
        return Stmt::Read;
    }
};

class Write: public Stmt {
public:
    std::optional<std::string> str;
    Expr* value;
    Location loc;
    Write(std::optional<std::string> str, Expr* value, const Location& loc):
          str(str), value(value), loc(loc) {}
    ~Write() override;
    void Format(std::ostream &os) const override;
    int SubclassKind() const override {
        return Stmt::Write;
    }
};

class Assign: public Stmt {
public:
    Expr* target;
    Expr* value;
    Location loc;
    Assign(Expr* target, Expr* value, const Location& loc): target(target),
           value(value), loc(loc) {}
    ~Assign() override;
    void Format(std::ostream &os) const override;
    int SubclassKind() const override {
        return Stmt::Assign;
    }
};

class For: public Stmt {
public:
    Stmt* initial;
    Expr* condition;
    Stmt* step;
    std::vector<Stmt*> body;
    Location loc;
    For(Stmt* initial, Expr* condition, Stmt* step, const std::vector<Stmt*>&
        body, const Location& loc): initial(initial), condition(condition),
        step(step), body(body), loc(loc) {}
    ~For() override;
    void Format(std::ostream &os) const override;
    int SubclassKind() const override {
        return Stmt::For;
    }
};

class While: public Stmt {
public:
    Expr* condition;
    std::vector<Stmt*> body;
    Location loc;
    While(Expr* condition, const std::vector<Stmt*>& body, const Location&
          loc): condition(condition), body(body), loc(loc) {}
    ~While() override;
    void Format(std::ostream &os) const override;
    int SubclassKind() const override {
        return Stmt::While;
    }
};

class Return: public Stmt {
public:
    Expr* value;
    Location loc;
    Return(Expr* value, const Location& loc): value(value), loc(loc) {}
    ~Return() override;
    void Format(std::ostream &os) const override;
    int SubclassKind() const override {
        return Stmt::Return;
    }
};

class If: public Stmt {
public:
    Expr* test;
    std::vector<Stmt*> body;
    std::vector<Stmt*> orelse;
    Location loc;
    If(Expr* test, const std::vector<Stmt*>& body, const std::vector<Stmt*>&
       orelse, const Location& loc): test(test), body(body), orelse(orelse),
       loc(loc) {}
    ~If() override;
    void Format(std::ostream &os) const override;
    int SubclassKind() const override {
        return Stmt::If;
    }
};

class ExprStmt: public Stmt {
public:
    Expr* value;
    Location loc;
    ExprStmt(Expr* value, const Location& loc): value(value), loc(loc) {}
    ~ExprStmt() override;
    void Format(std::ostream &os) const override;
    int SubclassKind() const override {
        return Stmt::ExprStmt;
    }
};

class Expr: public AST {
public:
    enum {BinOp, UnaryOp, Call, Num, Str, Char, Subscript, Name};
    virtual int SubclassKind() const = 0;
};

class BinOp: public Expr {
public:
    Expr* left;
    OperatorKind op;
    Expr* right;
    Location loc;
    BinOp(Expr* left, OperatorKind op, Expr* right, const Location& loc):
          left(left), op(op), right(right), loc(loc) {}
    ~BinOp() override;
    void Format(std::ostream &os) const override;
    int SubclassKind() const override {
        return Expr::BinOp;
    }
};

class UnaryOp: public Expr {
public:
    UnaryopKind op;
    Expr* operand;
    Location loc;
    UnaryOp(UnaryopKind op, Expr* operand, const Location& loc): op(op),
            operand(operand), loc(loc) {}
    ~UnaryOp() override;
    void Format(std::ostream &os) const override;
    int SubclassKind() const override {
        return Expr::UnaryOp;
    }
};

class Call: public Expr {
public:
    std::string func;
    std::vector<Expr*> args;
    Location loc;
    Call(const std::string& func, const std::vector<Expr*>& args, const
         Location& loc): func(func), args(args), loc(loc) {}
    ~Call() override;
    void Format(std::ostream &os) const override;
    int SubclassKind() const override {
        return Expr::Call;
    }
};

class Num: public Expr {
public:
    int n;
    Location loc;
    Num(int n, const Location& loc): n(n), loc(loc) {}
    ~Num() override;
    void Format(std::ostream &os) const override;
    int SubclassKind() const override {
        return Expr::Num;
    }
};

class Str: public Expr {
public:
    std::string s;
    Location loc;
    Str(const std::string& s, const Location& loc): s(s), loc(loc) {}
    ~Str() override;
    void Format(std::ostream &os) const override;
    int SubclassKind() const override {
        return Expr::Str;
    }
};

class Char: public Expr {
public:
    int c;
    Location loc;
    Char(int c, const Location& loc): c(c), loc(loc) {}
    ~Char() override;
    void Format(std::ostream &os) const override;
    int SubclassKind() const override {
        return Expr::Char;
    }
};

class Subscript: public Expr {
public:
    std::string name;
    Expr* index;
    ExprContextKind ctx;
    Location loc;
    Subscript(const std::string& name, Expr* index, ExprContextKind ctx, const
              Location& loc): name(name), index(index), ctx(ctx), loc(loc) {}
    ~Subscript() override;
    void Format(std::ostream &os) const override;
    int SubclassKind() const override {
        return Expr::Subscript;
    }
};

class Name: public Expr {
public:
    std::string id;
    ExprContextKind ctx;
    Location loc;
    Name(const std::string& id, ExprContextKind ctx, const Location& loc):
         id(id), ctx(ctx), loc(loc) {}
    ~Name() override;
    void Format(std::ostream &os) const override;
    int SubclassKind() const override {
        return Expr::Name;
    }
};

class VarType: public AST {
public:
    BasicTypeKind type;
    int is_array;
    int size;
    VarType(BasicTypeKind type, int is_array, int size): type(type),
            is_array(is_array), size(size) {}
    ~VarType() override;
    void Format(std::ostream &os) const override;
};

inline OperatorKind String2OperatorKind(const String &s) {
    if (s == "+") return OperatorKind::Add;

    if (s == "-") return OperatorKind::Sub;

    if (s == "*") return OperatorKind::Mult;

    if (s == "/") return OperatorKind::Div;

    if (s == "==") return OperatorKind::Eq;

    if (s == "!=") return OperatorKind::NotEq;

    if (s == "<") return OperatorKind::Lt;

    if (s == "<=") return OperatorKind::LtE;

    if (s == ">") return OperatorKind::Gt;

    if (s == ">=") return OperatorKind::GtE;

    assert(false && "not a member of OperatorKind");
}

inline UnaryopKind String2UnaryopKind(const String &s) {
    if (s == "+") return UnaryopKind::UAdd;

    if (s == "-") return UnaryopKind::USub;

    assert(false && "not a member of UnaryopKind");
}

inline BasicTypeKind String2BasicTypeKind(const String &s) {
    if (s == "int") return BasicTypeKind::Int;

    if (s == "char") return BasicTypeKind::Character;

    if (s == "void") return BasicTypeKind::Void;

    assert(false && "not a member of BasicTypeKind");
}



#endif