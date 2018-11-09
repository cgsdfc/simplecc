#include "grammar.h"

#define program 256
#define arglist 257
#define case_stmt 258
#define compound_stmt 259
#define condition 260
#define const_decl 261
#define const_item 262
#define decl_trailer 263
#define declaration 264
#define default_stmt 265
#define expr 266
#define factor 267
#define factor_trailer 268
#define for_stmt 269
#define if_stmt 270
#define integer 271
#define konstant 272
#define paralist 273
#define read_stmt 274
#define return_stmt 275
#define stmt 276
#define stmt_trailer 277
#define subscript 278
#define subscript2 279
#define switch_stmt 280
#define term 281
#define type_name 282
#define var_decl 283
#define var_item 284
#define write_stmt 285

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
