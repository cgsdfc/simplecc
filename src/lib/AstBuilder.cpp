#include "simplecc/AstBuilder.h"
#include "simplecc/AST.h"
#include "simplecc/ErrorManager.h"
#include "simplecc/Node.h"

#include <cassert>
#include <sstream>
#include <string>
#include <vector>

namespace {
using namespace simplecc;

Expr *MakeChar(Node *node) {
  return new Char(static_cast<int>(node->getValue()[1]), node->getLocation());
}

Expr *MakeNum(Node *node) {
  return new Num(std::stoi(node->getValue()), node->getLocation());
}
} // namespace

namespace simplecc {

class AstBuilder {
  Program *visit_program(Node *node) {
    assert(node->getType() == Symbol::program);
    std::vector<Decl *> decls;

    for (auto c : node->getChildren()) {
      if (c->getType() == Symbol::const_decl) {
        visit_const_decl(c, decls);
      } else if (c->getType() == Symbol::declaration) {
        visit_declaration(c, decls);
      } else {
        assert(c->getType() == Symbol::ENDMARKER);
        break;
      }
    }
    return new Program(decls);
  }

  void visit_const_decl(Node *node, std::vector<Decl *> &decls) {
    auto type_name = node->getChild(1);
    auto type = visit_type_name(type_name);

    for (int i = 2, len = node->getNumChildren() - 1; i < len; i += 2) {
      auto child = node->getChild(i);
      decls.push_back(visit_const_item(child, type));
    }
  }

  Decl *visit_const_item(Node *node, BasicTypeKind type) {
    auto name = node->FirstChild();
    auto konstant = node->getChild(2);
    Expr *val;

    if (konstant->getType() == Symbol::CHAR) {
      val = MakeChar(konstant);
    } else {
      assert(konstant->getType() == Symbol::integer);
      val = new Num(visit_integer(konstant), konstant->getLocation());
    }
    return new ConstDecl(type, val, name->getValue(), name->getLocation());
  }

  int visit_integer(Node *node) {
    std::ostringstream os;
    for (auto c : node->getChildren()) {
      os << c->getValue();
    }
    return std::stoi(os.str());
  }

  void visit_declaration(Node *node, std::vector<Decl *> &decls) {
    auto type_name = node->FirstChild();
    auto name = node->getChild(1);
    if (name->getValue() == "main") {
      std::vector<Decl *> fn_decls;
      std::vector<Stmt *> fn_stmts;

      auto type = visit_type_name(type_name);
      visit_compound_stmt(node->LastChild(), fn_decls, fn_stmts);
      decls.push_back(new FuncDef(type, {}, fn_decls, fn_stmts, "main",
                                  type_name->getLocation()));
    } else {
      auto decl_trailer = node->LastChild();
      assert(decl_trailer->getType() == Symbol::decl_trailer);
      visit_decl_trailer(decl_trailer, type_name, name, decls);
    }
  }

  void visit_decl_trailer(Node *node, Node *type_name, Node *name,
                          std::vector<Decl *> &decls) {
    auto first = node->FirstChild();
    auto type = visit_type_name(type_name);

    if (first->getValue() == ";") {
      decls.push_back(new VarDecl(type, false, 0, name->getValue(),
                                  type_name->getLocation()));
    } else if (first->getType() == Symbol::paralist ||
               first->getType() == Symbol::compound_stmt) {
      visit_funcdef(type, name->getValue(), node, type_name->getLocation(),
                    decls);
    } else {
      if (first->getType() == Symbol::subscript2) {
        auto size = visit_subscript2(first);
        decls.push_back(new VarDecl(type, true, size, name->getValue(),
                                    node->getLocation()));
      } else {
        decls.push_back(
            new VarDecl(type, false, 0, name->getValue(), node->getLocation()));
      }

      for (auto c : node->getChildren()) {
        if (c->getType() != Symbol::var_item)
          continue;
        decls.push_back(visit_var_item(c, type));
      }
    }
  }

  void visit_funcdef(BasicTypeKind return_type, const String &name,
                     Node *decl_trailer, const Location &location,
                     std::vector<Decl *> &decls) {
    std::vector<Decl *> paralist;
    std::vector<Decl *> fn_decls;
    std::vector<Stmt *> fn_stmts;

    if (decl_trailer->getNumChildren() > 1) {
      visit_paralist(decl_trailer->FirstChild(), paralist);
    }

    visit_compound_stmt(decl_trailer->LastChild(), fn_decls, fn_stmts);
    decls.push_back(
        new FuncDef(return_type, paralist, fn_decls, fn_stmts, name, location));
  }

  void visit_paralist(Node *node, std::vector<Decl *> &paralist) {
    int n_items = (node->getNumChildren() - 1) / 3;

    for (int i = 0; i < n_items; i++) {
      auto type_name = node->getChild(1 + i * 3);
      auto name = node->getChild(2 + i * 3);

      paralist.push_back(new ArgDecl(visit_type_name(type_name),
                                     name->getValue(),
                                     type_name->getLocation()));
    }
  }

  BasicTypeKind visit_type_name(Node *node) {
    return BasicTypeKindFromString(node->FirstChild()->getValue());
  }

  void visit_compound_stmt(Node *node, std::vector<Decl *> &fn_decls,
                           std::vector<Stmt *> &fn_stmts) {

    for (auto c : node->getChildren()) {
      if (c->getType() == Symbol::const_decl) {
        visit_const_decl(c, fn_decls);
      } else if (c->getType() == Symbol::var_decl) {
        visit_var_decl(c, fn_decls);
      } else if (c->getType() == Symbol::stmt) {
        visit_stmt(c, fn_stmts);
      }
    }
  }

  void visit_var_decl(Node *node, std::vector<Decl *> &decls) {
    auto type_name = node->FirstChild();
    auto type = visit_type_name(type_name);

    for (auto c : node->getChildren()) {
      if (c->getType() != Symbol::var_item)
        continue;
      decls.push_back(visit_var_item(c, type));
    }
  }

  Decl *visit_var_item(Node *node, BasicTypeKind type) {
    auto name = node->FirstChild();
    if (node->getNumChildren() == 1) {
      return new VarDecl(type, false, 0, name->getValue(), name->getLocation());
    } else {
      auto size = visit_subscript2(node->getChild(1));
      return new VarDecl(type, true, size, name->getValue(),
                         name->getLocation());
    }
  }

  void visit_stmt(Node *node, std::vector<Stmt *> &stmts) {
    auto first = node->FirstChild();
    if (first->getType() == Symbol::flow_stmt) {
      stmts.push_back(visit_flow_stmt(first));
    } else if (first->getType() == Symbol::NAME) {
      if (node->getNumChildren() == 2) {
        auto call = new Call(first->getValue(), {}, first->getLocation());
        stmts.push_back(new ExprStmt(call, node->getLocation()));
      } else {
        stmts.push_back(visit_stmt_trailer(node->getChild(1), first));
      }
    } else if (first->getValue() == "{") {
      for (auto c : node->getChildren()) {
        if (c->getType() == Symbol::stmt) {
          visit_stmt(c, stmts);
        }
      }
    } else {
      // discard the empty stmt -- ';'
      assert(first->getValue() == ";" && node->getNumChildren() == 1);
    }
  }

  Stmt *visit_stmt_trailer(Node *node, Node *name) {
    auto first = node->FirstChild();
    if (first->getType() == Symbol::arglist) {
      auto args = visit_arglist(first);
      auto call = new Call(name->getValue(), args, name->getLocation());
      return new ExprStmt(call, name->getLocation());
    } else if (first->getValue() == "[") {
      auto index = visit_expr(node->getChild(1));
      auto value = visit_expr(node->LastChild());
      auto subscript = new Subscript(
          name->getValue(), index, ExprContextKind::Store, node->getLocation());
      return new Assign(subscript, value, name->getLocation());
    } else {
      assert(first->getValue() == "=");
      auto value = visit_expr(node->LastChild());
      auto target = new Name(name->getValue(), ExprContextKind::Store,
                             name->getLocation());
      return new Assign(target, value, name->getLocation());
    }
  }

  Stmt *visit_flow_stmt(Node *node) {
    auto first = node->FirstChild();
    auto first_type = first->getType();
    if (first_type == Symbol::if_stmt)
      return visit_if_stmt(first);
    if (first_type == Symbol::for_stmt)
      return visit_for_stmt(first);
    if (first_type == Symbol::while_stmt)
      return visit_while_stmt(first);
    if (first_type == Symbol::return_stmt)
      return visit_return_stmt(first);
    if (first_type == Symbol::read_stmt)
      return visit_read_stmt(first);
    if (first_type == Symbol::write_stmt)
      return visit_write_stmt(first);
    assert(false && "should handle all flow_stmt");
  }

  Stmt *visit_if_stmt(Node *node) {
    auto condition = node->getChild(2);
    auto stmt = node->getChild(4);
    auto test = visit_condition(condition);
    std::vector<Stmt *> body, orelse;
    visit_stmt(stmt, body);
    if (node->getNumChildren() > 5)
      visit_stmt(node->LastChild(), orelse);
    return new If(test, body, orelse, node->getLocation());
  }

  Expr *visit_condition(Node *node) {
    bool has_cmpop = node->getNumChildren() == 3;
    return new BoolOp(visit_expr(node), has_cmpop, node->getLocation());
  }

  Stmt *visit_for_stmt(Node *node) {
    // initial: stmt
    auto name = node->getChild(2);
    auto expr = node->getChild(4);
    auto initial = new Assign(
        new Name(name->getValue(), ExprContextKind::Store, name->getLocation()),
        visit_expr(expr), name->getLocation());

    // condition: expr
    auto condition = visit_condition(node->getChild(6));

    // step: stmt
    auto target = node->getChild(8);
    auto name2 = node->getChild(10);
    auto op = node->getChild(11);
    auto num = node->getChild(12);
    assert(num->getType() == Symbol::NUMBER);
    auto expr1 = new Name(name2->getValue(), ExprContextKind::Load,
                          name2->getLocation());
    auto expr2 = MakeNum(num);
    auto expr3 = new BinOp(expr1, OperatorKindFromString(op->getValue()), expr2,
                           name2->getLocation());
    auto step = new Assign(new Name(target->getValue(), ExprContextKind::Store,
                                    target->getLocation()),
                           expr3, target->getLocation());

    // body: stmt*
    std::vector<Stmt *> body;
    visit_stmt(node->LastChild(), body);
    return new For(initial, condition, step, body, node->getLocation());
  }

  Stmt *visit_while_stmt(Node *node) {
    auto condition = visit_condition(node->getChild(2));
    std::vector<Stmt *> body;
    visit_stmt(node->LastChild(), body);
    return new While(condition, body, node->getLocation());
  }

  Stmt *visit_return_stmt(Node *node) {
    if (node->getNumChildren() == 1)
      return new Return(nullptr, node->getLocation());

    auto expr = visit_expr(node->getChild(2));
    return new Return(expr, node->getLocation());
  }

  Stmt *visit_read_stmt(Node *node) {
    std::vector<Expr *> names;
    for (int i = 1, len = node->getNumChildren(); i < len; i++) {
      auto child = node->getChild(i);
      if (child->getType() == Symbol::NAME) {
        names.push_back(new Name(child->getValue(), ExprContextKind::Store,
                                 child->getLocation()));
      }
    }
    return new Read(names, node->getLocation());
  }

  Stmt *visit_write_stmt(Node *node) {
    Expr *string = nullptr;
    Expr *expr = nullptr;

    for (auto c : node->getChildren()) {
      if (c->getType() == Symbol::expr)
        expr = visit_expr(c);
      else if (c->getType() == Symbol::STRING)
        string = new Str(c->getValue(), c->getLocation());
      // ignore other things
    }
    return new Write(string, expr, node->getLocation());
  }

  Expr *visit_expr(Node *node,
                   ExprContextKind context = ExprContextKind::Load) {

    if (node->getType() == Symbol::term || node->getType() == Symbol::expr ||
        node->getType() == Symbol::condition) {
      return visit_binop(node, context);
    } else {
      // factor
      assert(node->getType() == Symbol::factor);
      return visit_factor(node, context);
    }
  }

  /// factor: atom | unaryop factor
  Expr *visit_factor(Node *node, ExprContextKind context) {
    if (node->getNumChildren() == 1) {
      return visit_atom(node->FirstChild(), context);
    } else {
      auto first = node->FirstChild();
      auto op = UnaryopKindFromString(first->getValue());
      auto operand = visit_factor(node->getChild(1), context);
      return new UnaryOp(op, operand, first->getLocation());
    }
  }

  Expr *visit_binop(Node *node, ExprContextKind context) {
    auto result = visit_expr(node->FirstChild(), context);
    auto nops = (node->getNumChildren() - 1) / 2;

    for (int i = 0; i < nops; i++) {
      auto next_oper = node->getChild(i * 2 + 1);
      auto op = OperatorKindFromString(next_oper->getValue());
      auto tmp = visit_expr(node->getChild(i * 2 + 2), context);
      auto tmp_result = new BinOp(result, op, tmp, next_oper->getLocation());
      result = tmp_result;
    }
    return result;
  }

  Expr *visit_atom(Node *node, ExprContextKind context) {
    auto first = node->FirstChild();
    if (first->getType() == Symbol::NAME) {
      if (node->getNumChildren() == 1) {
        // single name
        return new Name(first->getValue(), context, first->getLocation());
      }
      // name with trailer: visit_trailer
      auto trailer = node->getChild(1);
      return visit_atom_trailer(trailer, first->getValue(), context);
    }
    if (first->getType() == Symbol::NUMBER) {
      return MakeNum(first);
    }
    if (first->getType() == Symbol::CHAR) {
      return MakeChar(first);
    } else {
      assert(first->getValue() == "(");
      auto value = visit_expr(node->getChild(1));
      return new ParenExpr(value, first->getLocation());
    }
  }

  Expr *visit_atom_trailer(Node *node, const String &name,
                           ExprContextKind context) {
    auto first = node->FirstChild();
    if (first->getType() == Symbol::arglist) {
      // no empty arglist
      auto args = visit_arglist(first);
      return new Call(name, args, node->getLocation());
    } else {
      assert(first->getValue() == "[");
      auto index = visit_expr(node->getChild(1));
      return new Subscript(name, index, context, node->getLocation());
    }
  }

  std::vector<Expr *> visit_arglist(Node *node) {
    std::vector<Expr *> args;
    for (auto c : node->getChildren()) {
      if (c->getType() == Symbol::expr) {
        args.push_back(visit_expr(c));
      }
    }
    return args;
  }

  int visit_subscript2(Node *node) {
    return std::stoi(node->getChild(1)->getValue());
  }

public:
  Program *Build(const Node *N) { return visit_program(const_cast<Node *>(N)); }
};
} // namespace simplecc

Program *simplecc::BuildAstFromNode(const Node *node) {
  return AstBuilder().Build(node);
}
