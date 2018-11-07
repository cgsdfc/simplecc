#include "grammar.h"

#define program 256
#define arglist 257
#define case_stmt 258
#define compound_stmt 259
#define condition 260
#define const_decl 261
#define const_item 262
#define constant 263
#define decl_trailer 264
#define declaration 265
#define default_stmt 266
#define expr 267
#define factor 268
#define for_stmt 269
#define if_stmt 270
#define integer 271
#define paralist 272
#define read_stmt 273
#define relation_op 274
#define return_stmt 275
#define stmt 276
#define stmt_trailer 277
#define subscript 278
#define subscript2 279
#define switch_stmt 280
#define term 281
#define trailer 282
#define type_name 283
#define var_decl 284
#define var_item 285
#define write_stmt 286

#define NAME 0
#define OP 1
#define ERRORTOKEN 2
#define ENDMARKER 3
#define CHAR 4
#define NUMBER 5
#define STRING 6

extern Grammar CompilerGrammar;
extern const char *TokenNames[];
