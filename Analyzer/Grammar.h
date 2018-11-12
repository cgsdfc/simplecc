#ifndef GRAMMAR_H
#define GRAMMAR_H
#include "gramdef.h"

enum class Symbol {
    program = 256,
    arglist = 257,
    compound_stmt = 258,
    condition = 259,
    const_decl = 260,
    const_item = 261,
    decl_trailer = 262,
    declaration = 263,
    expr = 264,
    factor = 265,
    factor_trailer = 266,
    flow_stmt = 267,
    for_stmt = 268,
    if_stmt = 269,
    integer = 270,
    paralist = 271,
    read_stmt = 272,
    return_stmt = 273,
    stmt = 274,
    stmt_trailer = 275,
    subscript2 = 276,
    term = 277,
    type_name = 278,
    var_decl = 279,
    var_item = 280,
    while_stmt = 281,
    write_stmt = 282,
    NAME = 0,
    OP = 1,
    ERRORTOKEN = 2,
    ENDMARKER = 3,
    CHAR = 4,
    NUMBER = 5,
    STRING = 6,
};


#define NT_OFFSET 256
extern Grammar CompilerGrammar;
extern const char *TokenNames[], *SymbolNames[];

#endif
