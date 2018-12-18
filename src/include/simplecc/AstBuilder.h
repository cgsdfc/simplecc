#ifndef AST_BUILDER_H
#define AST_BUILDER_H

#include "AST.h"
#include <vector>

namespace simplecc {
class Node;

class AstBuilder {
  Expr *MakeChar(Node *node);
  Expr *MakeNum(Node *node);
  Program *visit_program(Node *node);

  void visit_const_decl(Node *node, std::vector<Decl *> &decls);

  Decl *visit_const_item(Node *node, BasicTypeKind type);

  int visit_integer(Node *node);

  void visit_declaration(Node *node, std::vector<Decl *> &decls);

  void visit_decl_trailer(Node *node, Node *type_name, Node *name,
                          std::vector<Decl *> &decls);

  void visit_funcdef(BasicTypeKind return_type, const String &name,
                     Node *decl_trailer, const Location &location,
                     std::vector<Decl *> &decls);

  void visit_paralist(Node *node, std::vector<Decl *> &paralist);

  BasicTypeKind visit_type_name(Node *node);

  void visit_compound_stmt(Node *node, std::vector<Decl *> &fn_decls,
                           std::vector<Stmt *> &fn_stmts);

  void visit_var_decl(Node *node, std::vector<Decl *> &decls);

  Decl *visit_var_item(Node *node, BasicTypeKind type);

  void visit_stmt(Node *node, std::vector<Stmt *> &stmts);

  Stmt *visit_stmt_trailer(Node *node, Node *name);

  Stmt *visit_flow_stmt(Node *node);

  Stmt *visit_if_stmt(Node *node);

  Expr *visit_condition(Node *node);

  Stmt *visit_for_stmt(Node *node);

  Stmt *visit_while_stmt(Node *node);

  Stmt *visit_return_stmt(Node *node);

  Stmt *visit_read_stmt(Node *node);

  Stmt *visit_write_stmt(Node *node);

  Expr *visit_expr(Node *node, ExprContextKind context = ExprContextKind::Load);

  /// factor: atom | unaryop factor
  Expr *visit_factor(Node *node, ExprContextKind context);

  Expr *visit_binop(Node *node, ExprContextKind context);

  Expr *visit_atom(Node *node, ExprContextKind context);

  Expr *visit_atom_trailer(Node *node, const String &name,
                           ExprContextKind context);

  std::vector<Expr *> visit_arglist(Node *node);

  int visit_subscript2(Node *node);

public:
  Program *Build(const Node *N) { return visit_program(const_cast<Node *>(N)); }
};

Program *BuildAstFromNode(const Node *node);
} // namespace simplecc

#endif
