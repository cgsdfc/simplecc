#ifndef AST_BUILDER_H
#define AST_BUILDER_H

#include "AST.h"
#include <vector>

namespace simplecc {
class Node;

/// This class builds the AST from the CST according to the Grammar rules.
class AstBuilder {
  Expr *MakeChar(Node *node);
  Expr *MakeNum(Node *node);

  /// program: const_decl* declaration* ENDMARKER
  Program *visit_program(Node *node);

  /// const_decl: 'const' type_name const_item (',' const_item)* ';'
  void visit_const_decl(Node *node, std::vector<Decl *> &decls);

  /// const_item: NAME '=' (integer | CHAR)
  Decl *visit_const_item(Node *node, BasicTypeKind type);

  /// integer: ['+'|'-'] NUMBER
  int visit_integer(Node *node);

  /// declaration: type_name (NAME decl_trailer | 'main' '(' ')' compound_stmt )
  void visit_declaration(Node *node, std::vector<Decl *> &decls);

  /// decl_trailer: [ paralist ] compound_stmt | [subscript2] (',' var_item)* ';'
  void visit_decl_trailer(Node *node, Node *type_name, Node *name,
                          std::vector<Decl *> &decls);

  void visit_funcdef(BasicTypeKind return_type, const String &name,
                     Node *decl_trailer, const Location &location,
                     std::vector<Decl *> &decls);

  /// paralist: '(' type_name NAME (',' type_name NAME)* ')'
  void visit_paralist(Node *node, std::vector<Decl *> &paralist);

  /// type_name: 'int' | 'char' | 'void'
  BasicTypeKind visit_type_name(Node *node);

  /// compound_stmt: '{' const_decl* var_decl* stmt* '}'
  void visit_compound_stmt(Node *node, std::vector<Decl *> &fn_decls,
                           std::vector<Stmt *> &fn_stmts);

  /// var_decl: type_name var_item (',' var_item)* ';'
  void visit_var_decl(Node *node, std::vector<Decl *> &decls);

  /// var_item: NAME [subscript2]
  Decl *visit_var_item(Node *node, BasicTypeKind type);

  /// stmt: flow_stmt | '{' stmt* '}' | NAME [stmt_trailer] ';' | ';'
  void visit_stmt(Node *node, std::vector<Stmt *> &stmts);

  /// stmt_trailer: arglist | ['[' expr ']'] '=' expr
  Stmt *visit_stmt_trailer(Node *node, Node *name);

  /// flow_stmt: if_stmt | for_stmt | while_stmt |
  ///            read_stmt ';' | write_stmt ';' | return_stmt ';'
  Stmt *visit_flow_stmt(Node *node);

  /// if_stmt: 'if' '(' condition ')' stmt ['else' stmt]
  Stmt *visit_if_stmt(Node *node);

  /// condition: expr ('<'|'<='|'>'|'>='|'!='|'==') expr | expr
  Expr *visit_condition(Node *node);

  /// for_stmt: ('for' '(' NAME '=' expr ';' condition ';'
  //            NAME '=' NAME ('+'|'-') NUMBER ')' stmt)
  Stmt *visit_for_stmt(Node *node);

  /// while_stmt: 'while' '(' condition ')' stmt
  Stmt *visit_while_stmt(Node *node);

  /// return_stmt: 'return' ['(' expr ')']
  Stmt *visit_return_stmt(Node *node);

  /// read_stmt: 'scanf' '(' NAME (',' NAME)* ')'
  Stmt *visit_read_stmt(Node *node);

  /// write_stmt: 'printf' '(' (expr|STRING [',' expr]) ')'
  Stmt *visit_write_stmt(Node *node);

  /// expr: term (('+'|'-') term)*
  Expr *visit_expr(Node *node, ExprContextKind context = ExprContextKind::Load);

  /// factor: atom | unaryop factor
  Expr *visit_factor(Node *node, ExprContextKind context);

  /// term: factor (('*'|'/') factor)*
  Expr *visit_binop(Node *node, ExprContextKind context);

  /// atom: NAME [ atom_trailer ] | NUMBER | CHAR | '(' expr ')'
  Expr *visit_atom(Node *node, ExprContextKind context);

  /// atom_trailer: '[' expr ']' | arglist
  Expr *visit_atom_trailer(Node *node, const String &name,
                           ExprContextKind context);

  /// arglist: '(' expr (',' expr)* ')'
  std::vector<Expr *> visit_arglist(Node *node);

  /// subscript2: '[' NUMBER ']'
  int visit_subscript2(Node *node);

public:
  Program *Build(const Node *N) { return visit_program(const_cast<Node *>(N)); }
};

Program *BuildAstFromNode(const Node *node);
} // namespace simplecc

#endif
