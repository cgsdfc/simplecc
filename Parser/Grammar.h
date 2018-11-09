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
#define konstant 270
#define paralist 271
#define read_stmt 272
#define return_stmt 273
#define stmt 274
#define subscript 275
#define subscript2 276
#define term 277
#define type_name 278
#define var_decl 279
#define var_item 280
#define while_stmt 281
#define write_stmt 282

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
