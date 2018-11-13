#include "AST.h"

Program::~Program() {
    for (auto v: decls) delete v;
}
void Program::Format(std::ostream &os) const {
    os << "Program(";
    os << "decls=";
    os << decls;
    os << ")";
}

Location getLocation(Decl *v) {
    if (auto x = subclass_cast<ConstDecl>(v))
        return x->loc;
    if (auto x = subclass_cast<VarDecl>(v))
        return x->loc;
    if (auto x = subclass_cast<FuncDef>(v))
        return x->loc;
    assert(false && "Decl");
}

ConstDecl::~ConstDecl() {
    delete value;
}
void ConstDecl::Format(std::ostream &os) const {
    os << "ConstDecl(";
    os << "type=";
    os << type;
    os << ", ";
    os << "name=";
    os << name;
    os << ", ";
    os << "value=";
    os << *value;
    os << ", ";
    os << "loc=";
    os << loc;
    os << ")";
}

VarDecl::~VarDecl() {
    delete type;
}
void VarDecl::Format(std::ostream &os) const {
    os << "VarDecl(";
    os << "type=";
    os << *type;
    os << ", ";
    os << "name=";
    os << name;
    os << ", ";
    os << "loc=";
    os << loc;
    os << ")";
}

FuncDef::~FuncDef() {
    for (auto v: args) delete v;
    for (auto v: decls) delete v;
    for (auto v: stmts) delete v;
}
void FuncDef::Format(std::ostream &os) const {
    os << "FuncDef(";
    os << "return_type=";
    os << return_type;
    os << ", ";
    os << "name=";
    os << name;
    os << ", ";
    os << "args=";
    os << args;
    os << ", ";
    os << "decls=";
    os << decls;
    os << ", ";
    os << "stmts=";
    os << stmts;
    os << ", ";
    os << "loc=";
    os << loc;
    os << ")";
}

Arg::~Arg() {
}
void Arg::Format(std::ostream &os) const {
    os << "Arg(";
    os << "type=";
    os << type;
    os << ", ";
    os << "name=";
    os << name;
    os << ", ";
    os << "loc=";
    os << loc;
    os << ")";
}

Location getLocation(Stmt *v) {
    if (auto x = subclass_cast<Read>(v))
        return x->loc;
    if (auto x = subclass_cast<Write>(v))
        return x->loc;
    if (auto x = subclass_cast<Assign>(v))
        return x->loc;
    if (auto x = subclass_cast<For>(v))
        return x->loc;
    if (auto x = subclass_cast<While>(v))
        return x->loc;
    if (auto x = subclass_cast<Return>(v))
        return x->loc;
    if (auto x = subclass_cast<If>(v))
        return x->loc;
    if (auto x = subclass_cast<ExprStmt>(v))
        return x->loc;
    assert(false && "Stmt");
}

Read::~Read() {
}
void Read::Format(std::ostream &os) const {
    os << "Read(";
    os << "names=";
    os << names;
    os << ", ";
    os << "loc=";
    os << loc;
    os << ")";
}

Write::~Write() {
    if (value) delete value;
}
void Write::Format(std::ostream &os) const {
    os << "Write(";
    os << "str=";
    if (str) os << str.value(); else os << "None";
    os << ", ";
    os << "value=";
    if (value) os << value; else os << "None";
    os << ", ";
    os << "loc=";
    os << loc;
    os << ")";
}

Assign::~Assign() {
    delete target;
    delete value;
}
void Assign::Format(std::ostream &os) const {
    os << "Assign(";
    os << "target=";
    os << *target;
    os << ", ";
    os << "value=";
    os << *value;
    os << ", ";
    os << "loc=";
    os << loc;
    os << ")";
}

For::~For() {
    delete initial;
    delete condition;
    delete step;
    for (auto v: body) delete v;
}
void For::Format(std::ostream &os) const {
    os << "For(";
    os << "initial=";
    os << *initial;
    os << ", ";
    os << "condition=";
    os << *condition;
    os << ", ";
    os << "step=";
    os << *step;
    os << ", ";
    os << "body=";
    os << body;
    os << ", ";
    os << "loc=";
    os << loc;
    os << ")";
}

While::~While() {
    delete condition;
    for (auto v: body) delete v;
}
void While::Format(std::ostream &os) const {
    os << "While(";
    os << "condition=";
    os << *condition;
    os << ", ";
    os << "body=";
    os << body;
    os << ", ";
    os << "loc=";
    os << loc;
    os << ")";
}

Return::~Return() {
    if (value) delete value;
}
void Return::Format(std::ostream &os) const {
    os << "Return(";
    os << "value=";
    if (value) os << value; else os << "None";
    os << ", ";
    os << "loc=";
    os << loc;
    os << ")";
}

If::~If() {
    delete test;
    for (auto v: body) delete v;
    for (auto v: orelse) delete v;
}
void If::Format(std::ostream &os) const {
    os << "If(";
    os << "test=";
    os << *test;
    os << ", ";
    os << "body=";
    os << body;
    os << ", ";
    os << "orelse=";
    os << orelse;
    os << ", ";
    os << "loc=";
    os << loc;
    os << ")";
}

ExprStmt::~ExprStmt() {
    delete value;
}
void ExprStmt::Format(std::ostream &os) const {
    os << "ExprStmt(";
    os << "value=";
    os << *value;
    os << ", ";
    os << "loc=";
    os << loc;
    os << ")";
}

Location getLocation(Expr *v) {
    if (auto x = subclass_cast<BinOp>(v))
        return x->loc;
    if (auto x = subclass_cast<UnaryOp>(v))
        return x->loc;
    if (auto x = subclass_cast<Call>(v))
        return x->loc;
    if (auto x = subclass_cast<Num>(v))
        return x->loc;
    if (auto x = subclass_cast<Str>(v))
        return x->loc;
    if (auto x = subclass_cast<Char>(v))
        return x->loc;
    if (auto x = subclass_cast<Subscript>(v))
        return x->loc;
    if (auto x = subclass_cast<Name>(v))
        return x->loc;
    assert(false && "Expr");
}

BinOp::~BinOp() {
    delete left;
    delete right;
}
void BinOp::Format(std::ostream &os) const {
    os << "BinOp(";
    os << "left=";
    os << *left;
    os << ", ";
    os << "op=";
    os << op;
    os << ", ";
    os << "right=";
    os << *right;
    os << ", ";
    os << "loc=";
    os << loc;
    os << ")";
}

UnaryOp::~UnaryOp() {
    delete operand;
}
void UnaryOp::Format(std::ostream &os) const {
    os << "UnaryOp(";
    os << "op=";
    os << op;
    os << ", ";
    os << "operand=";
    os << *operand;
    os << ", ";
    os << "loc=";
    os << loc;
    os << ")";
}

Call::~Call() {
    for (auto v: args) delete v;
}
void Call::Format(std::ostream &os) const {
    os << "Call(";
    os << "func=";
    os << func;
    os << ", ";
    os << "args=";
    os << args;
    os << ", ";
    os << "loc=";
    os << loc;
    os << ")";
}

Num::~Num() {
}
void Num::Format(std::ostream &os) const {
    os << "Num(";
    os << "n=";
    os << n;
    os << ", ";
    os << "loc=";
    os << loc;
    os << ")";
}

Str::~Str() {
}
void Str::Format(std::ostream &os) const {
    os << "Str(";
    os << "s=";
    os << s;
    os << ", ";
    os << "loc=";
    os << loc;
    os << ")";
}

Char::~Char() {
}
void Char::Format(std::ostream &os) const {
    os << "Char(";
    os << "c=";
    os << c;
    os << ", ";
    os << "loc=";
    os << loc;
    os << ")";
}

Subscript::~Subscript() {
    delete index;
}
void Subscript::Format(std::ostream &os) const {
    os << "Subscript(";
    os << "name=";
    os << name;
    os << ", ";
    os << "index=";
    os << *index;
    os << ", ";
    os << "ctx=";
    os << ctx;
    os << ", ";
    os << "loc=";
    os << loc;
    os << ")";
}

Name::~Name() {
}
void Name::Format(std::ostream &os) const {
    os << "Name(";
    os << "id=";
    os << id;
    os << ", ";
    os << "ctx=";
    os << ctx;
    os << ", ";
    os << "loc=";
    os << loc;
    os << ")";
}

VarType::~VarType() {
}
void VarType::Format(std::ostream &os) const {
    os << "VarType(";
    os << "type=";
    os << type;
    os << ", ";
    os << "is_array=";
    os << is_array;
    os << ", ";
    os << "size=";
    os << size;
    os << ")";
}

