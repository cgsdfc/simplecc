#ifndef SIMPLECC_PARSE_GRAMMAR_H
#define SIMPLECC_PARSE_GRAMMAR_H
#include "simplecc/Parse/GramDef.h"

namespace simplecc {
enum class Symbol {
  program = 256,
  arglist = 257,
  atom = 258,
  atom_trailer = 259,
  compound_stmt = 260,
  condition = 261,
  const_decl = 262,
  const_item = 263,
  decl_trailer = 264,
  declaration = 265,
  expr = 266,
  factor = 267,
  flow_stmt = 268,
  for_stmt = 269,
  if_stmt = 270,
  integer = 271,
  paralist = 272,
  read_stmt = 273,
  return_stmt = 274,
  stmt = 275,
  stmt_trailer = 276,
  subscript2 = 277,
  term = 278,
  type_name = 279,
  var_decl = 280,
  var_item = 281,
  while_stmt = 282,
  write_stmt = 283,
  NAME = 0,
  OP = 1,
  ERRORTOKEN = 2,
  ENDMARKER = 3,
  CHAR = 4,
  NUMBER = 5,
  STRING = 6,
};

constexpr const unsigned NT_OFFSET = 256;
extern Grammar CompilerGrammar;
extern const char *TokenNames[], *SymbolNames[];
} // namespace simplecc

#endif // SIMPLECC_PARSE_GRAMMAR_H