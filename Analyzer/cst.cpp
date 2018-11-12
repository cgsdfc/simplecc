#include "cst.h"

#include <algorithm>
#include <tuple>

class TransformerVisitor {
public:

  Program *visit_program(Node *node) {
    assert(node->type == Symbol::program);
    std::vector<Decl*> decls;

    for (auto c: node->children) {
      if (c->type == Symbol::const_decl) {
        visit_const_decl(c, decls);
      }
      else if (c->type == Symbol::declaration) {
        visit_declaration(c, decls);
      }
      else {
        assert(c->type == Symbol::ENDMARKER);
        break;
      }
    }
    return new Program(decls);
  }

  void visit_const_decl(Node *node, std::vector<Decl*> &decls) {
    auto type_name = node->children[1];
    auto type = visit_type_name(type_name);

    for (int i = 2, len = node->children.size() - 1; i < len; i += 2) {
      auto child = node->children[i];
      String name;
      Expr *val;
      std::tie(name, val) = visit_const_item(child);
      decls.push_back(new ConstDecl(type, name, val, node->location));
    }

  }

  std::pair<String, Expr*>
    visit_const_item(Node *node) {
      auto name = node->children[0];
      auto konstant = node->children[2];
      Expr *val;

      if (konstant->type == Symbol::CHAR) {
        val = new Char(std::stoi(node->value), node->location);
      }
      else {
        assert(konstant->type == Symbol::integer);
        val = new Num(visit_integer(konstant), konstant->location);
      }

      return {name->value, val};
    }

  int visit_integer(Node *node) {
    std::ostringstream os;
    for (auto c: node->children) {
      os << c->value;
    }
    return std::stoi(os.str());
  }

  void visit_declaration(Node *node, std::vector<Decl*> &decls) {
    auto type_name = node->children[0];
    auto name = node->children[1];
    if (name->value == "main") {
      std::vector<Decl*> fn_decls;
      std::vector<Stmt*> fn_stmts;

      auto type = visit_type_name(type_name);
      visit_compound_stmt(node->LastChild(), fn_decls, fn_stmts);
      decls.push_back(
          new FuncDef(type, "main", {}, fn_decls, fn_stmts, type_name->location));
    }
    else {
      auto decl_trailer = node->LastChild();
      assert(decl_trailer->type == Symbol::decl_trailer);
      visit_decl_trailer(decl_trailer, type_name, name, decls);
    }
  }

  void visit_decl_trailer(Node *node, Node *type_name, Node *name,
      std::vector<Decl*> &decls) {
    auto first = node->FirstChild();
    auto type = visit_type_name(type_name);

    if (first->value == ";") {
      auto var_type = new VarType(type, false, 0);
      decls.push_back(new VarDecl(var_type, name->value, type_name->location));
    }
    else if (first->type == Symbol::paralist ||
        first->type == Symbol::compound_stmt) {
      visit_funcdef(type, name->value, node, type_name->location, decls);
    }
    else {
      if (first->type == Symbol::subscript2) {
        auto var_type = new VarType(type, true, visit_subscript2(first));
        decls.push_back(new VarDecl(var_type, name->value, node->location));
      }
      else {
        auto var_type = new VarType(type, false, 0);
        decls.push_back(new VarDecl(var_type, name->value, node->location));
      }

      for (auto c: node->children) {
        if (c->type != Symbol::var_item)
          continue;
        String name;
        bool is_array;
        int size;
        std::tie(name, is_array, size) = visit_var_item(c);
        auto var_type = new VarType(type, is_array, size);
        decls.push_back(new VarDecl(var_type, name, type_name->location));
      }
    }
  }

  void visit_funcdef(BasicTypeKind return_type, const String &name,
      Node *decl_trailer, const Location &location,
      std::vector<Decl*> &decls) {
    std::vector<Arg*> paralist;
    std::vector<Decl*> fn_decls;
    std::vector<Stmt*> fn_stmts;

    if (decl_trailer->children.size() > 1) {
      visit_paralist(decl_trailer->FirstChild(), paralist);
    }

    visit_compound_stmt(decl_trailer->LastChild(), fn_decls, fn_stmts);
    decls.push_back(new FuncDef(
          return_type, name, paralist, fn_decls, fn_stmts, location));
  }

  void visit_paralist(Node *node, std::vector<Arg*> &paralist) {
    int n_items = (node->children.size() - 1) / 3;

    for (int i = 0; i < n_items; i++) {
      auto type_name = node->children[1 + i * 3];
      auto name = node->children[2 + i * 3];

      paralist.push_back(new Arg(visit_type_name(type_name),
            name->value, type_name->location));
    }
  }

  BasicTypeKind visit_type_name(Node *node) {
    return String2BasicTypeKind(node->FirstChild()->value);
  }

  void visit_compound_stmt(Node *node,
    std::vector<Decl*> &fn_decls,
    std::vector<Stmt*> &fn_stmts) {

    for (auto c: node->children) {
      if (c->type == Symbol::const_decl) {
        visit_const_decl(c, fn_decls);
      }
      else if (c->type == Symbol::var_decl) {
        visit_var_decl(c, fn_decls);
      }
      else if (c->type == Symbol::stmt) {
        visit_stmt(c, fn_stmts);
      }
    }
  }

  void visit_var_decl(Node *node, std::vector<Decl*> &decls) {
    auto type_name = node->FirstChild();
    auto type = visit_type_name(type_name);

    for (auto c: node->children) {
      if (c->type != Symbol::var_item)
        continue;
      String name;
      bool is_array;
      int size;
      std::tie(name, is_array, size) = visit_var_item(c);
      auto var_type = new VarType(type, is_array, size);
      decls.push_back(new VarDecl(var_type, name, type_name->location));
    }
  }

  std::tuple<String, bool, int>
    visit_var_item(Node *node) {
      auto name = node->FirstChild()->value;
      if (node->children.size() == 1) {
        return {name, false, 0};
      }
      return {name, true, visit_subscript2(node->children[1])};
    }

  void visit_stmt(Node *node, std::vector<Stmt*> &stmts) {
    auto first = node->FirstChild();
    if (first->type == Symbol::flow_stmt) {
      stmts.push_back(visit_flow_stmt(first));
    }
    else if (first->type == Symbol::expr) {
      if (node->children.size() == 2) {
        auto expr1 = visit_expr(first);
        stmts.push_back(new ExprStmt(expr1, node->location));
      } else {
        assert(node->children[1]->value == "=");
        auto expr1 = visit_expr(first, ExprContextKind::Store);
        auto expr2 = visit_expr(*(node->children.end() - 2));
        stmts.push_back(new Assign(expr1, expr2, node->location));
      }
    }
    else if (first->value == "{") {
      for (auto c: node->children) {
        if (c->type == Symbol::stmt) {
          visit_stmt(c, stmts);
        }
      }
    }
    else {
      // discard the empty stmt -- ';'
      assert(first->value == ";" && node->children.size() == 1);
    }
  }

  Stmt *visit_flow_stmt(Node *node) {
    auto first = node->FirstChild();
    auto first_type = first->type;
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
    auto condition = node->children[2];
    auto stmt = node->children[4];
    auto test = visit_condition(condition);
    std::vector<Stmt*> body, orelse;
    visit_stmt(stmt, body);
    if (node->children.size() > 5)
      visit_stmt(node->LastChild(), orelse);
    return new If(test, body, orelse, node->location);
  }

  Expr *visit_condition(Node *node) {
    return visit_expr(node);
  }

  Stmt *visit_for_stmt(Node *node) {
    // initial: stmt
    auto name = node->children[2];
    auto expr = node->children[4];
    auto initial = new Assign(
        new Name(name->value, ExprContextKind::Store, name->location),
        visit_expr(expr),
        name->location);

    // condition: expr
    auto condition = visit_expr(node->children[6]);

    // step: stmt
    auto target = node->children[8];
    auto name2 = node->children[10];
    auto op = node->children[11];
    auto num = node->children[12];
    assert(num->type == Symbol::NUMBER);
    auto expr1 = new Name(name2->value, ExprContextKind::Load, name2->location);
    auto expr2 = new Num(std::stoi(num->value), num->location);
    auto expr3 = new BinOp(expr1,
        String2OperatorKind(op->value), expr2, name2->location);
    auto step = new Assign(
        new Name(target->value, ExprContextKind::Store, target->location),
        expr3, target->location);


    // body: stmt*
    std::vector<Stmt*> body;
    visit_stmt(node->LastChild(), body);
    return new For(initial, condition, step, body, node->location);
  }

  Stmt *visit_while_stmt(Node *node) {
    auto condition = visit_condition(node->children[2]);
    std::vector<Stmt*> body;
    visit_stmt(node->LastChild(), body);
    return new While(condition, body, node->location);
  }

  Stmt *visit_return_stmt(Node  *node) {
    if (node->children.size() == 2)
      return new Return(nullptr, node->location);

    auto expr = visit_expr(node->children[2]);
    return new Return(expr, node->location);
  }

  Stmt *visit_read_stmt(Node *node) {
    std::vector<String> names;
    for (auto b = node->children.begin() + 1, e = node->children.end();
        b != e; b++) {
      auto child = *b;
      if (child->type == Symbol::NAME) {
        names.push_back(child->value);
      }
    }
    return new Read(names, node->location);
  }

  Stmt *visit_write_stmt(Node *node) {
    std::optional<String> string;
    Expr *expr = nullptr;

    for (auto c: node->children) {
      if (c->type == Symbol::expr)
        expr = visit_expr(c);
      else if (c->type == Symbol::STRING)
        string = c->value;
      // ignore other things
    }
    return new Write(string, expr, node->location);
  }

   Expr *visit_expr(Node *node,
       ExprContextKind context=ExprContextKind::Load) {
     std::optional<UnaryopKind> unaryop;

     if (node->type == Symbol::expr) {
       auto first = node->FirstChild();
       if (first->type == Symbol::OP &&
           (first->value == "-" || first->value == "+")) {
         unaryop = String2UnaryopKind(first->value);
       }
     }

     Expr *result;
     if (node->type == Symbol::term ||
         node->type == Symbol::expr ||
         node->type == Symbol::condition) {
       result = visit_binop(node, context);
     }
     else {
       assert(node->type == Symbol::factor);
       result = visit_factor(node, context);
     }

     if (unaryop.has_value())
       return new UnaryOp(unaryop.value(), result, node->location);
     return result;
   }

   Expr *visit_binop(Node *node, ExprContextKind context) {
     auto result = visit_expr(node->FirstChild(), context);
     auto nops = (node->children.size() - 1) / 2;

     for (int i = 0; i < nops; i++) {
       auto next_oper = node->children[i * 2 + 1];
       auto op = String2OperatorKind( next_oper->value );
       auto tmp = visit_expr(node->children[i * 2 + 2], context);
       auto tmp_result = new BinOp(result, op, tmp, next_oper->location);
       result = tmp_result;
     }
     return result;
   }

    Expr *visit_factor(Node *node, ExprContextKind context) {
        auto first = node->FirstChild();
        if (first->type == Symbol::NAME) {
            if (node->children.size() == 1) {
                // single name
                return new Name(first->value, context, first->location);
            }
            // name with trailer: visit_trailer
            auto trailer = node->children[1];
            return visit_factor_trailer(trailer, first->value, context);
        }
        if (first->type == Symbol::NUMBER) {
            return new Num(std::stoi(first->value), first->location);
        }
        if (first->type == Symbol::CHAR) {
            return new Char(std::stoi(first->value), first->location);
        }
        else {
            assert(first->value == "(");
            return visit_expr(node->children[1]);
        }
    }

    Expr *visit_factor_trailer(Node *node, const String &name,
        ExprContextKind context) {
        auto first = node->FirstChild();
        if (first->type == Symbol::arglist) {
          // no empty arglist
          auto args = visit_arglist(first);
          return new Call(name, args, node->location);
        }
        else {
            assert(first->value == "[");
            auto index = visit_expr(node->children[1]);
            return new Subscript(name, index, context, node->location);
        }
    }

    std::vector<Expr*> visit_arglist(Node *node) {
      std::vector<Expr*> args;
      for (auto c: node->children) {
        if (c->type == Symbol::expr) {
          args.push_back(visit_expr(c));
        }
      }
      return args;
    }

    int visit_subscript2(Node *node) {
      return std::stoi(node->children[1]->value);
    }

};

Program *NodeToAst(Node *node) {
  return TransformerVisitor().visit_program(node);
}
