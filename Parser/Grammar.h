#include "grammar.h"

#define program 256
#define arglist 257
#define compound_stmt 258
#define condition 259
#define const_decl 260
#define const_item 261
#define decl_trailer 262
#define declaration 263
#define expr 264
#define factor 265
#define factor_trailer 266
#define for_stmt 267
#define if_stmt 268
#define integer 269
#define paralist 270
#define read_stmt 271
#define return_stmt 272
#define stmt 273
#define subscript2 274
#define term 275
#define type_name 276
#define var_decl 277
#define var_item 278
#define while_stmt 279
#define write_stmt 280

#define NAME 0
#define OP 1
#define ERRORTOKEN 2
#define ENDMARKER 3
#define CHAR 4
#define NUMBER 5
#define STRING 6

#define NT_OFFSET 256
extern Grammar CompilerGrammar;
extern const char *TokenNames[], *SymbolNames[];
