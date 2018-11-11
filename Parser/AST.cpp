#include "AST.h"

Program::~Program() {
    for (auto v: decls) delete v;
}


ConstDecl::~ConstDecl() {
    delete value;
}

VarDecl::~VarDecl() {
    delete type;
}

FuncDef::~FuncDef() {
    for (auto v: args) delete v;
    for (auto v: decls) delete v;
    for (auto v: stmts) delete v;
}

Arg::~Arg() {
}


Read::~Read() {
}

Write::~Write() {
    if (value) delete value;
}

Assign::~Assign() {
    delete target;
    delete value;
}

For::~For() {
    delete initial;
    delete condition;
    delete step;
    for (auto v: body) delete v;
}

While::~While() {
    delete condition;
    for (auto v: body) delete v;
}

Return::~Return() {
    if (value) delete value;
}

If::~If() {
    delete test;
    for (auto v: body) delete v;
    for (auto v: orelse) delete v;
}

ExprStmt::~ExprStmt() {
    delete value;
}


BinOp::~BinOp() {
    delete left;
    delete right;
}

UnaryOp::~UnaryOp() {
    delete operand;
}

Call::~Call() {
    for (auto v: args) delete v;
}

Num::~Num() {
}

Str::~Str() {
}

Char::~Char() {
}

Subscript::~Subscript() {
    delete index;
}

Name::~Name() {
}





VarType::~VarType() {
}


