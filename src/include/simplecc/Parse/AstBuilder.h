#ifndef SIMPLECC_PARSE_ASTBUILDER_H
#define SIMPLECC_PARSE_ASTBUILDER_H

#include "simplecc/Parse/AST.h"
#include <vector>

namespace simplecc {
class Node;

/// This class builds the AST from the CST according to the Grammar rules.
class AstBuilder {
  Expr *makeChar(Node *N);
  Expr *makeNum(Node *N);

  /// program: const_decl* declaration* ENDMARKER
  Program *visit_program(Node *N);

  /// const_decl: 'const' type_name const_item (',' const_item)* ';'
  void visit_const_decl(Node *N, std::vector<Decl *> &Decls);

  /// const_item: NAME '=' (integer | CHAR)
  Decl *visit_const_item(Node *N, BasicTypeKind Ty);

  /// integer: ['+'|'-'] NUMBER
  int visit_integer(Node *N);

  /// declaration: type_name (NAME decl_trailer | 'main' '(' ')' compound_stmt )
  void visit_declaration(Node *N, std::vector<Decl *> &Decls);

  /// decl_trailer: [ paralist ] compound_stmt | [subscript2] (',' var_item)*
  /// ';'
  void visit_decl_trailer(Node *N, Node *TypeName, Node *Name,
                          std::vector<Decl *> &Decls);

  Decl *visit_funcdef(BasicTypeKind RetTy, String Name, Node *decl_trailer,
                      const Location &L);

  /// paralist: '(' type_name NAME (',' type_name NAME)* ')'
  void visit_paralist(Node *N, std::vector<Decl *> &ParamList);

  /// type_name: 'int' | 'char' | 'void'
  BasicTypeKind visit_type_name(Node *N);

  /// compound_stmt: '{' const_decl* var_decl* stmt* '}'
  void visit_compound_stmt(Node *N, std::vector<Decl *> &Decls,
                           std::vector<Stmt *> &Stmts);

  /// var_decl: type_name var_item (',' var_item)* ';'
  void visit_var_decl(Node *N, std::vector<Decl *> &Decls);

  /// var_item: NAME [subscript2]
  Decl *visit_var_item(Node *N, BasicTypeKind Ty);

  /// stmt: flow_stmt | '{' stmt* '}' | NAME [stmt_trailer] ';' | ';'
  void visit_stmt(Node *N, std::vector<Stmt *> &Stmts);

  /// stmt_trailer: arglist | ['[' expr ']'] '=' expr
  Stmt *visit_stmt_trailer(Node *N, Node *Name);

  /// flow_stmt: if_stmt | for_stmt | while_stmt |
  ///            read_stmt ';' | write_stmt ';' | return_stmt ';'
  Stmt *visit_flow_stmt(Node *N);

  /// if_stmt: 'if' '(' condition ')' stmt ['else' stmt]
  Stmt *visit_if_stmt(Node *N);

  /// condition: expr ('<'|'<='|'>'|'>='|'!='|'==') expr | expr
  Expr *visit_condition(Node *N);

  /// for_stmt: ('for' '(' NAME '=' expr ';' condition ';'
  //            NAME '=' NAME ('+'|'-') NUMBER ')' stmt)
  Stmt *visit_for_stmt(Node *N);

  /// while_stmt: 'while' '(' condition ')' stmt
  Stmt *visit_while_stmt(Node *N);

  /// return_stmt: 'return' ['(' expr ')']
  Stmt *visit_return_stmt(Node *N);

  /// read_stmt: 'scanf' '(' NAME (',' NAME)* ')'
  Stmt *visit_read_stmt(Node *N);

  /// write_stmt: 'printf' '(' (expr|STRING [',' expr]) ')'
  Stmt *visit_write_stmt(Node *N);

  /// expr: term (('+'|'-') term)*
  Expr *visit_expr(Node *N, ExprContextKind Context = ExprContextKind::Load);

  /// factor: atom | unaryop factor
  Expr *visit_factor(Node *N, ExprContextKind Context);

  /// term: factor (('*'|'/') factor)*
  Expr *visit_binop(Node *N, ExprContextKind Context);

  /// atom: NAME [ atom_trailer ] | NUMBER | CHAR | '(' expr ')'
  Expr *visit_atom(Node *N, ExprContextKind Context);

  /// atom_trailer: '[' expr ']' | arglist
  Expr *visit_atom_trailer(Node *N, const String &Name,
                           ExprContextKind Context);

  /// arglist: '(' expr (',' expr)* ')'
  std::vector<Expr *> visit_arglist(Node *N);

  /// subscript2: '[' NUMBER ']'
  int visit_subscript2(Node *N);

public:
  Program *Build(const Node *N) { return visit_program(const_cast<Node *>(N)); }
};
} // namespace simplecc

#endif
