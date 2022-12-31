// MIT License
// 
// Copyright (c) 2018 Cong Feng.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef SIMPLECC_PARSE_ASTBUILDER_H
#define SIMPLECC_PARSE_ASTBUILDER_H
#include "simplecc/AST/AST.h"
#include "simplecc/Support/ErrorManager.h"
#include <string>
#include <vector>

namespace simplecc {
class Node;

/// This class builds the AST from the CST according to the Grammar rules.
class ASTBuilder {
  /// Create a CharExpr from a CHAR node.
  CharExpr *makeCharExpr(Node *N);

  /// Create a NumExpr from a NUMBER node.
  NumExpr *makeNumExpr(Node *N);

  /// program: const_decl* declaration* ENDMARKER
  ProgramAST *visit_program(std::string Filename, Node *N);

  /// const_decl: 'const' type_name const_item (',' const_item)* ';'
  void visit_const_decl(Node *N, std::vector<DeclAST *> &Decls);

  /// const_item: NAME '=' (integer | CHAR)
  DeclAST *visit_const_item(Node *N, BasicTypeKind Ty);

  /// integer: ['+'|'-'] NUMBER
  int visit_integer(Node *N);

  /// declaration: type_name (NAME decl_trailer | 'main' '(' ')' compound_stmt )
  void visit_declaration(Node *N, std::vector<DeclAST *> &Decls);

  /// decl_trailer: [ paralist ] compound_stmt | [subscript2] (',' var_item)* ';'
  void visit_decl_trailer(Node *N, Node *TypeName, Node *Name,
                          std::vector<DeclAST *> &Decls);

  DeclAST *visit_funcdef(BasicTypeKind RetTy, std::string Name,
                         Node *decl_trailer, Location L);

  /// paralist: '(' type_name NAME (',' type_name NAME)* ')'
  void visit_paralist(Node *N, std::vector<ArgDecl *> &ParamList);

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

  /// Handle conversion from string to integer.
  /// If the resultant integer exceeds the range of int,
  /// report error through EM.
  int evaluate_integer(const std::string &Str, Location L);
public:
  /// Create an AST from the parse tree and the filename.
  /// On error, return nullptr and print an error.
  std::unique_ptr<ProgramAST, DeleteAST>
      Build(const std::string &Filename, const Node *N);
private:
  ErrorManager EM;
};
} // namespace simplecc
#endif // SIMPLECC_PARSE_ASTBUILDER_H