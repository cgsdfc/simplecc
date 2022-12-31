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

#include "simplecc/Parse/ASTBuilder.h"
#include "simplecc/Parse/Node.h"
#include <sstream>
#include <iterator>

using namespace simplecc;

ProgramAST *ASTBuilder::visit_program(std::string Filename, Node *N) {
  assert(N->getType() == Symbol::program);
  std::vector<DeclAST *> Decls;

  for (auto C : N->getChildren()) {
    if (C->getType() == Symbol::const_decl) {
      visit_const_decl(C, Decls);
    } else if (C->getType() == Symbol::declaration) {
      visit_declaration(C, Decls);
    } else {
      assert(C->getType() == Symbol::ENDMARKER);
      break;
    }
  }
  return new ProgramAST(std::move(Filename), std::move(Decls));
}

void ASTBuilder::visit_const_decl(Node *N, std::vector<DeclAST *> &Decls) {
  auto TypeName = N->getChild(1);
  auto Ty = visit_type_name(TypeName);

  for (unsigned i = 2, len = N->getNumChildren() - 1; i < len; i += 2) {
    auto Child = N->getChild(i);
    Decls.push_back(visit_const_item(Child, Ty));
  }
}

DeclAST *ASTBuilder::visit_const_item(Node *N, BasicTypeKind Ty) {
  auto name = N->getFirstChild();
  auto constant = N->getChild(2);
  ExprAST *Val;

  if (constant->getType() == Symbol::CHAR) {
    Val = makeCharExpr(constant);
  } else {
    assert(constant->getType() == Symbol::integer);
    Val = new NumExpr(visit_integer(constant), constant->getLocation());
  }
  return new ConstDecl(Ty, name->getValue(), Val, name->getLocation());
}

int ASTBuilder::visit_integer(Node *N) {
  std::ostringstream OS;
  std::transform(N->begin(), N->end(), std::ostream_iterator<std::string>(OS),
                 [](Node *C) { return C->getValue(); });
  return evaluate_integer(OS.str(), N->getLocation());
}

void ASTBuilder::visit_declaration(Node *N, std::vector<DeclAST *> &Decls) {
  auto TypeName = N->getFirstChild();
  auto name = N->getChild(1);
  if (name->getValue() == "main") {
    std::vector<DeclAST *> FnDecls;
    std::vector<StmtAST *> FnStmts;
    auto Ty = visit_type_name(TypeName);
    visit_compound_stmt(N->getLastChild(), FnDecls, FnStmts);
    Decls.push_back(new FuncDef(Ty, {}, std::move(FnDecls), std::move(FnStmts),
                                "main", TypeName->getLocation()));
    return;
  }

  auto decl_trailer = N->getLastChild();
  assert(decl_trailer->getType() == Symbol::decl_trailer);
  visit_decl_trailer(decl_trailer, TypeName, name, Decls);
}

std::vector<ExprAST *> ASTBuilder::visit_arglist(Node *N) {
  std::vector<ExprAST *> Args;
  for (auto C : N->getChildren()) {
    if (C->getType() == Symbol::expr) {
      Args.push_back(visit_expr(C));
    }
  }
  return std::move(Args);
}

ExprAST *ASTBuilder::visit_atom_trailer(Node *N, const std::string &Name,
                                        ExprContextKind Context) {
  auto first = N->getFirstChild();
  if (first->getType() == Symbol::arglist) {
    // no empty arglist
    std::vector<ExprAST *> Args = visit_arglist(first);
    return new CallExpr(Name, Args, N->getLocation());
  }

  assert(first->getValue() == "[");
  auto index = visit_expr(N->getChild(1));
  return new SubscriptExpr(Name, index, Context, N->getLocation());
}

ExprAST *ASTBuilder::visit_atom(Node *N, ExprContextKind Context) {
  auto first = N->getFirstChild();
  if (first->getType() == Symbol::NAME) {
    if (N->getNumChildren() == 1) {
      // single name
      return new NameExpr(first->getValue(), Context, first->getLocation());
    }
    // name with trailer: visit_trailer
    auto trailer = N->getChild(1);
    return visit_atom_trailer(trailer, first->getValue(), Context);
  }

  if (first->getType() == Symbol::NUMBER) {
    return makeNumExpr(first);
  }

  if (first->getType() == Symbol::CHAR) {
    return makeCharExpr(first);
  }

  assert(first->getValue() == "(");
  auto value = visit_expr(N->getChild(1));
  return new ParenExpr(value, first->getLocation());
}

StmtAST *ASTBuilder::visit_write_stmt(Node *N) {
  ExprAST *S = nullptr;
  ExprAST *E = nullptr;

  for (auto C : N->getChildren()) {
    if (C->getType() == Symbol::expr)
      E = visit_expr(C);
    else if (C->getType() == Symbol::STRING)
      S = new StrExpr(C->getValue(), C->getLocation());
    // ignore other things
  }
  return new WriteStmt(S, E, N->getLocation());
}

void ASTBuilder::visit_decl_trailer(Node *N, Node *TypeName, Node *Name,
                                    std::vector<DeclAST *> &Decls) {

  auto first = N->getFirstChild();
  auto Ty = visit_type_name(TypeName);

  if (first->getValue() == ";") {
    Decls.push_back(
        new VarDecl(Ty, Name->getValue(), false, 0, TypeName->getLocation()));
    return;
  }

  if (first->getType() == Symbol::paralist ||
      first->getType() == Symbol::compound_stmt) {
    Decls.push_back(
        visit_funcdef(Ty, Name->getValue(), N, TypeName->getLocation()));
    return;
  }

  bool IsArray = first->getType() == Symbol::subscript2;
  int ArraySize = IsArray ? visit_subscript2(first) : 0;
  Decls.push_back(
      new VarDecl(Ty, Name->getValue(), IsArray, ArraySize, N->getLocation()));

  for (auto C : N->getChildren()) {
    if (C->getType() != Symbol::var_item)
      continue;
    Decls.push_back(visit_var_item(C, Ty));
  }
}

StmtAST *ASTBuilder::visit_return_stmt(Node *N) {
  if (N->getNumChildren() == 1)
    return new ReturnStmt(nullptr, N->getLocation());

  auto expr = visit_expr(N->getChild(2));
  return new ReturnStmt(expr, N->getLocation());
}

void ASTBuilder::visit_stmt(Node *N, std::vector<StmtAST *> &Stmts) {
  auto first = N->getFirstChild();
  if (first->getType() == Symbol::flow_stmt) {
    return Stmts.push_back(visit_flow_stmt(first));
  }

  if (first->getType() == Symbol::NAME) {
    if (N->getNumChildren() == 2) {
      auto call = new CallExpr(first->getValue(), {}, first->getLocation());
      return Stmts.push_back(new ExprStmt(call, N->getLocation()));
    }
    return Stmts.push_back(visit_stmt_trailer(N->getChild(1), first));
  }

  if (first->getValue() == "{") {
    for (auto C : N->getChildren()) {
      if (C->getType() == Symbol::stmt) {
        visit_stmt(C, Stmts);
      }
    }
    return;
  }
  // discard the empty stmt -- ';'
  assert(first->getValue() == ";" && N->getNumChildren() == 1);
}

StmtAST *ASTBuilder::visit_flow_stmt(Node *N) {
  auto first = N->getFirstChild();
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

StmtAST *ASTBuilder::visit_if_stmt(Node *N) {
  auto condition = N->getChild(2);
  auto stmt = N->getChild(4);
  auto test = visit_condition(condition);
  std::vector<StmtAST *> body, orelse;
  visit_stmt(stmt, body);
  if (N->getNumChildren() > 5)
    visit_stmt(N->getLastChild(), orelse);
  return new IfStmt(test, std::move(body), std::move(orelse), N->getLocation());
}

ExprAST *ASTBuilder::visit_binop(Node *N, ExprContextKind Context) {
  auto result = visit_expr(N->getFirstChild(), Context);
  auto NumBinOp = (N->getNumChildren() - 1) / 2;

  for (unsigned i = 0; i < NumBinOp; i++) {
    auto NextOp = N->getChild(i * 2 + 1);
    auto op = OperatorKindFromString(NextOp->getValue());
    auto tmp = visit_expr(N->getChild(i * 2 + 2), Context);
    auto tmp_result = new BinOpExpr(result, op, tmp, NextOp->getLocation());
    result = tmp_result;
  }
  return result;
}

DeclAST *ASTBuilder::visit_funcdef(BasicTypeKind RetTy, std::string Name,
                                   Node *decl_trailer, Location L) {
  std::vector<ArgDecl *> ParamList;
  std::vector<DeclAST *> FnDecls;
  std::vector<StmtAST *> FnStmts;

  if (decl_trailer->getNumChildren() > 1) {
    visit_paralist(decl_trailer->getFirstChild(), ParamList);
  }

  visit_compound_stmt(decl_trailer->getLastChild(), FnDecls, FnStmts);
  return new FuncDef(RetTy, std::move(ParamList),
                     std::move(FnDecls),
                     std::move(FnStmts), std::move(Name), L);
}

ExprAST *ASTBuilder::visit_condition(Node *N) {
  bool has_cmpop = N->getNumChildren() == 3;
  return new BoolOpExpr(visit_expr(N), has_cmpop, N->getLocation());
}

StmtAST *ASTBuilder::visit_for_stmt(Node *N) {
  // initial: stmt
  auto Nn = N->getChild(2);
  auto expr = N->getChild(4);
  auto Initial = new AssignStmt(
      /* target */ new NameExpr(Nn->getValue(), ExprContextKind::Store,
                                Nn->getLocation()),
      /* value */ visit_expr(expr), /* loc */ Nn->getLocation());

  // condition: expr
  auto Cond = visit_condition(N->getChild(6));

  // step: stmt
  auto target = N->getChild(8);
  auto name2 = N->getChild(10);
  auto op = N->getChild(11);
  auto num = N->getChild(12);
  assert(num->getType() == Symbol::NUMBER);
  auto L = new NameExpr(name2->getValue(), ExprContextKind::Load,
                        name2->getLocation());
  auto R = makeNumExpr(num);
  auto BO = new BinOpExpr(
      /* left */ L,
      /* op */ OperatorKindFromString(op->getValue()),
      /* right */ R, name2->getLocation());
  auto Step = new AssignStmt(
      /* target */ new NameExpr(target->getValue(), ExprContextKind::Store,
                                target->getLocation()),
      /* value */ BO,
      /* loc */ target->getLocation());

  // body: stmt*
  std::vector<StmtAST *> Body;
  visit_stmt(N->getLastChild(), Body);
  return new ForStmt(Initial, Cond, Step, std::move(Body), N->getLocation());
}

void ASTBuilder::visit_paralist(Node *N, std::vector<ArgDecl *> &ParamList) {
  size_t NumItems = (N->getNumChildren() - 1) / 3;

  for (unsigned i = 0; i < NumItems; i++) {
    auto TypeName = N->getChild(1 + i * 3);
    auto Name = N->getChild(2 + i * 3);

    ParamList.push_back(new ArgDecl(
        /* type */ visit_type_name(TypeName),
        /* name */ Name->getValue(),
        /* loc */ TypeName->getLocation()));
  }
}

ExprAST *ASTBuilder::visit_factor(Node *N, ExprContextKind Context) {
  if (N->getNumChildren() == 1) {
    return visit_atom(N->getFirstChild(), Context);
  }

  auto first = N->getFirstChild();
  auto op = UnaryopKindFromString(first->getValue());
  auto operand = visit_factor(N->getChild(1), Context);
  return new UnaryOpExpr(op, operand, first->getLocation());
}

StmtAST *ASTBuilder::visit_stmt_trailer(Node *N, Node *Name) {
  auto first = N->getFirstChild();
  if (first->getType() == Symbol::arglist) {
    std::vector<ExprAST *> Args = visit_arglist(first);
    auto C =
        new CallExpr(Name->getValue(), std::move(Args), Name->getLocation());
    return new ExprStmt(C, Name->getLocation());

  } else if (first->getValue() == "[") {
    auto Idx = visit_expr(N->getChild(1));
    auto Val = visit_expr(N->getLastChild());
    auto SB = new SubscriptExpr(Name->getValue(), Idx, ExprContextKind::Store,
                                N->getLocation());
    return new AssignStmt(SB, Val, Name->getLocation());

  } else {
    assert(first->getValue() == "=");
    auto Val = visit_expr(N->getLastChild());
    auto Target = new NameExpr(Name->getValue(), ExprContextKind::Store,
                               Name->getLocation());
    return new AssignStmt(Target, Val, Name->getLocation());
  }
}

void ASTBuilder::visit_compound_stmt(Node *N, std::vector<DeclAST *> &FnDecls,
                                     std::vector<StmtAST *> &FnStmts) {
  for (auto C : N->getChildren()) {
    switch (C->getType()) {
    case Symbol::const_decl:visit_const_decl(C, FnDecls);
      break;
    case Symbol::var_decl:visit_var_decl(C, FnDecls);
      break;
    case Symbol::stmt:visit_stmt(C, FnStmts);
      break;
      // discard left brace & right brace.
    default:continue;
    }
  }
}

StmtAST *ASTBuilder::visit_read_stmt(Node *N) {
  std::vector<NameExpr *> Names;
  std::for_each(std::next(N->begin()), N->end(), [&Names](Node *Child) {
    if (Child->getType() == Symbol::NAME) {
      Names.push_back(new NameExpr(Child->getValue(), ExprContextKind::Store,
                                   Child->getLocation()));
    }
  });
  return new ReadStmt(std::move(Names), N->getLocation());
}

ExprAST *ASTBuilder::visit_expr(Node *N, ExprContextKind Context) {

  if (N->getType() == Symbol::term || N->getType() == Symbol::expr ||
      N->getType() == Symbol::condition) {
    return visit_binop(N, Context);
  }
  // factor
  assert(N->getType() == Symbol::factor);
  return visit_factor(N, Context);
}

void ASTBuilder::visit_var_decl(Node *N, std::vector<DeclAST *> &Decls) {
  auto TypeName = N->getFirstChild();
  auto Ty = visit_type_name(TypeName);
  for (auto C : N->getChildren()) {
    if (C->getType() != Symbol::var_item)
      continue;
    Decls.push_back(visit_var_item(C, Ty));
  }
}

DeclAST *ASTBuilder::visit_var_item(Node *N, BasicTypeKind Ty) {
  auto name = N->getFirstChild();
  bool IsArray = N->getNumChildren() > 1;
  int Size = IsArray ? visit_subscript2(N->getChild(1)) : 0;
  return new VarDecl(Ty,
      /* name */ name->getValue(),
      /* IsArray */ IsArray,
      /* size */ Size, name->getLocation());
}

StmtAST *ASTBuilder::visit_while_stmt(Node *N) {
  auto Cond = visit_condition(N->getChild(2));
  std::vector<StmtAST *> Body;
  visit_stmt(N->getLastChild(), Body);
  return new WhileStmt(Cond, std::move(Body), N->getLocation());
}

BasicTypeKind ASTBuilder::visit_type_name(Node *N) {
  return BasicTypeKindFromString(N->getFirstChild()->getValue());
}

int ASTBuilder::visit_subscript2(Node *N) {
  auto Child = N->getChild(1);
  return evaluate_integer(Child->getValue(), Child->getLocation());
}

CharExpr *ASTBuilder::makeCharExpr(Node *N) {
  assert(N->getType() == Symbol::CHAR);
  return new CharExpr(static_cast<int>(N->getValue()[1]), N->getLocation());
}

NumExpr *ASTBuilder::makeNumExpr(Node *N) {
  assert(N->getType() == Symbol::NUMBER);
  auto loc = N->getLocation();
  return new NumExpr(evaluate_integer(N->getValue(), loc), loc);
}

int ASTBuilder::evaluate_integer(const std::string &Str, Location L) {
  try {
    return std::stoi(Str);
  } catch (std::out_of_range &E) {
    EM.Error(L, "integer out of range:", Str);
    return 0;
  }
}

std::unique_ptr<ProgramAST, DeleteAST>
ASTBuilder::Build(const std::string &Filename, const Node *N) {
  std::unique_ptr<ProgramAST, DeleteAST> Ptr(visit_program(Filename, const_cast<Node *>(N)));
  if (EM.IsOk())
    return std::move(Ptr);
  return nullptr;
}
