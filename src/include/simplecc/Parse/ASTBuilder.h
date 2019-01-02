#ifndef SIMPLECC_PARSE_ASTBUILDER_H
#define SIMPLECC_PARSE_ASTBUILDER_H

#include "simplecc/Parse/AST.h"
#include <string>
#include <vector>

namespace simplecc {
class Node;

/// This class builds the AST from the CST according to the Grammar rules.
class ASTBuilder {
  ExprAST *makeChar(Node *N);
  ExprAST *makeNum(Node *N);

  /// program: const_decl* declaration* ENDMARKER
  Program *visit_program(std::string Filename, Node *N);

  /// const_decl: 'const' type_name const_item (',' const_item)* ';'
  void visit_const_decl(Node *N, std::vector<DeclAST *> &Decls);

  /// const_item: NAME '=' (integer | CHAR)
  DeclAST *visit_const_item(Node *N, BasicTypeKind Ty);

  /// integer: ['+'|'-'] NUMBER
  int visit_integer(Node *N);

  /// declaration: type_name (NAME decl_trailer | 'main' '(' ')' compound_stmt )
  void visit_declaration(Node *N, std::vector<DeclAST *> &Decls);

  /// decl_trailer: [ paralist ] compound_stmt | [subscript2] (',' var_item)*
  /// ';'
  void visit_decl_trailer(Node *N, Node *TypeName, Node *Name,
                          std::vector<DeclAST *> &Decls);

  DeclAST *visit_funcdef(BasicTypeKind RetTy, std::string Name,
                         Node *decl_trailer, const Location &L);

  /// paralist: '(' type_name NAME (',' type_name NAME)* ')'
  void visit_paralist(Node *N, std::vector<DeclAST *> &ParamList);

  /// type_name: 'int' | 'char' | 'void'
  BasicTypeKind visit_type_name(Node *N);

  /// compound_stmt: '{' const_decl* var_decl* stmt* '}'
  void visit_compound_stmt(Node *N, std::vector<DeclAST *> &Decls,
                           std::vector<StmtAST *> &Stmts);

  /// var_decl: type_name var_item (',' var_item)* ';'
  void visit_var_decl(Node *N, std::vector<DeclAST *> &Decls);

  /// var_item: NAME [subscript2]
  DeclAST *visit_var_item(Node *N, BasicTypeKind Ty);

  /// stmt: flow_stmt | '{' stmt* '}' | NAME [stmt_trailer] ';' | ';'
  void visit_stmt(Node *N, std::vector<StmtAST *> &Stmts);

  /// stmt_trailer: arglist | ['[' expr ']'] '=' expr
  StmtAST *visit_stmt_trailer(Node *N, Node *Name);

  /// flow_stmt: if_stmt | for_stmt | while_stmt |
  ///            read_stmt ';' | write_stmt ';' | return_stmt ';'
  StmtAST *visit_flow_stmt(Node *N);

  /// if_stmt: 'if' '(' condition ')' stmt ['else' stmt]
  StmtAST *visit_if_stmt(Node *N);

  /// condition: expr ('<'|'<='|'>'|'>='|'!='|'==') expr | expr
  ExprAST *visit_condition(Node *N);

  /// for_stmt: ('for' '(' NAME '=' expr ';' condition ';'
  //            NAME '=' NAME ('+'|'-') NUMBER ')' stmt)
  StmtAST *visit_for_stmt(Node *N);

  /// while_stmt: 'while' '(' condition ')' stmt
  StmtAST *visit_while_stmt(Node *N);

  /// return_stmt: 'return' ['(' expr ')']
  StmtAST *visit_return_stmt(Node *N);

  /// read_stmt: 'scanf' '(' NAME (',' NAME)* ')'
  StmtAST *visit_read_stmt(Node *N);

  /// write_stmt: 'printf' '(' (expr|STRING [',' expr]) ')'
  StmtAST *visit_write_stmt(Node *N);

  /// expr: term (('+'|'-') term)*
  ExprAST *visit_expr(Node *N, ExprContextKind Context = ExprContextKind::Load);

  /// factor: atom | unaryop factor
  ExprAST *visit_factor(Node *N, ExprContextKind Context);

  /// term: factor (('*'|'/') factor)*
  ExprAST *visit_binop(Node *N, ExprContextKind Context);

  /// atom: NAME [ atom_trailer ] | NUMBER | CHAR | '(' expr ')'
  ExprAST *visit_atom(Node *N, ExprContextKind Context);

  /// atom_trailer: '[' expr ']' | arglist
  ExprAST *visit_atom_trailer(Node *N, const std::string &Name,
                              ExprContextKind Context);

  /// arglist: '(' expr (',' expr)* ')'
  std::vector<ExprAST *> visit_arglist(Node *N);

  /// subscript2: '[' NUMBER ']'
  int visit_subscript2(Node *N);

public:
  Program *Build(const std::string &Filename, const Node *N) {
    return visit_program(Filename, const_cast<Node *>(N));
  }
};
} // namespace simplecc

#endif
