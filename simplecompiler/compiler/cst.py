"""Transform a CST to an AST"""
from pprint import pprint
import sys

from simplecompiler.compiler import AST
from simplecompiler.compiler.AST import expr_context
from simplecompiler.compiler import Symbol as sym

from simplecompiler.compiler import astpretty
# register our AST intead of Python's
astpretty.customize(AST.AST, expr_context)

from simplecompiler.compiler.astpretty import pformat as ast_pformat

__all__ = ["build_ast", "astpretty_pprint"]


def astpretty_pprint(ast_node, file):
    """Pretty print an ast_node to file"""
    print(ast_pformat(ast_node), file=file)


class VisitorBase:
    """Base class for visiting a CST."""

    def __init__(self):
        self.cache = {}

    def visit(self, node, *args, **kwargs):
        assert node is not None
        symbol = sym.tok_name[node.type]
        meth = self.cache.get(symbol)
        if meth is None:
            methname = 'visit_' + symbol
            try:
                meth = getattr(self, methname)
            except AttributeError:
                # don't silent the absent of meth
                raise NotImplementedError(methname)
            else:
                self.cache[symbol] = meth
        return meth(node, *args, **kwargs)


class TransformerVisitor(VisitorBase):
    """Visitor transforming CST to AST"""

    def visit_program(self, node):
        assert node.type == sym.program
        decls = []
        for c in node.children[:-1]:
            # generator
            decls.extend(self.visit(c))
        return AST.Program(decls)

    def visit_const_decl(self, node):
        # strip tailing semicolon
        _, type_name, *const_items = node.children[:-1]
        type = self.visit(type_name)
        # visit_const_item
        name_vals = map(self.visit, const_items[::2])  # skip comma
        for name, val in name_vals:
            # type all the same
            yield AST.ConstDecl(type, val, name, node.context)

    def visit_const_item(self, node):
        name, _, konstant = node.children
        if konstant.type == sym.CHAR:
            val = AST.Char(node.value, node.context)
        else:
            assert konstant.type == sym.integer
            val = AST.Num(self.visit(konstant), konstant.context)
        return name.value, val

    def visit_integer(self, node):
        # extract optional sign and digits
        num = ''.join(map(lambda c: c.value, node.children))
        return int(num)

    def visit_declaration(self, node):
        type_name, *trailer = node.children
        name = trailer[0]
        if name.value == 'main':
            # main() {}
            compound_stmt = self.visit(trailer[-1])
            type = self.visit(type_name)
            return [AST.FuncDef(type, [], *compound_stmt, 'main',
                type_name.context)]
        else:
            assert trailer[-1].type == sym.decl_trailer
            decl_trailer = trailer[-1]
            return list(self.visit(decl_trailer, type_name, name))

    def visit_decl_trailer(self, node, type_name, name):
        # generator: need to be list()
        first = node.first_child
        type = self.visit(type_name)
        if first.value == ';':  # a single non-array var_decl
            yield AST.VarDecl(type, False, 0, name.value, type_name.context)
        elif first.type in (sym.paralist, sym.compound_stmt):
            yield self.visit_funcdef(type, name.value, node.children, type_name.context)
        else:  # complex var_decl
            if first.type == sym.subscript2:  # first item is an array
                # strip subscript2 and semicolon
                var_items = node.children[1:-1]
                # visit_subscript2
                yield AST.VarDecl(type, True, self.visit(first), name.value, node.context)
            else:  # first item is a basic_type
                var_items = node.children[:-1]  # strip semicolon
                yield AST.VarDecl(type, False, 0, name.value, node.context)

            # handle (',', var_items)*
            for child in var_items[1::2]:
                # visit_var_item
                yield self.visit(child, type)

    def visit_funcdef(self, return_type, name, children, context):
        # children is the children of decl_trailer
        # name is already an identifier
        paralist = [] if len(children) == 1 else list(self.visit(children[0]))
        compound_stmt = children[-1]  # always the last one
        decls, stmts = self.visit(compound_stmt)
        return AST.FuncDef(return_type, paralist, decls, stmts, name, context)

    def visit_paralist(self, node):
        # strip leading ( and tailing )
        child = iter(node.children[1:-1])

        while True:
            type_name = next(child)
            type = self.visit(type_name)
            name = next(child).value
            yield AST.arg(type, name, type_name.context)
            next(child)  # skip ','

    def visit_type_name(self, node):
        name = node.first_child.value
        return AST.string2basic_type[name]

    def visit_compound_stmt(self, node):
        decls = []
        stmts = []
        for c in node.children[1:-1]:
            visited = self.visit(c)
            if c.type in (sym.const_decl, sym.var_decl):
                decls.extend(list(visited))
            else:
                assert c.type == sym.stmt
                stmts.extend(list(visited))
        return decls, stmts

    def visit_var_decl(self, node):
        type_name, *var_items = node.children[:-1]
        type = self.visit(type_name)
        # var_items is a varying sequence
        for child in var_items[::2]:
            # visit_var_item
            yield self.visit(child, type)

    def visit_var_item(self, node, type):
        name = node.first_child
        if len(node.children) == 1:
            # name, is_array, size, context
            return AST.VarDecl(type, False, 0, name.value, name.context)
        else:
            # visit_subscript2
            size = self.visit(node.children[1])
            return AST.VarDecl(type, True, size, name.value, name.context)

    def visit_stmt(self, node):
        first = node.first_child
        if first.type == sym.flow_stmt:
            # flow_stmt is always one stmt
            yield self.visit(node.first_child.first_child)
        elif first.type == sym.NAME:
            if len(node.children) == 2:
                # single Name is a Call
                call = AST.Call(first.value, [], first.context)
                yield AST.ExprStmt(call, node.context)
            else:  # stmt_trailer
                yield self.visit(node.children[1], first)
        elif first.value == '{':
            for child in node.children[1:-1]:
                yield from self.visit(child)
        else:
            # make sure we handle all cases.
            assert first.value == ';' and len(node.children) == 1, node

    def visit_stmt_trailer(self, node, name):
        first = node.first_child
        store = expr_context.Store
        if first.type == sym.arglist:
            args = self.visit(first)
            call = AST.Call(name.value, args, name.context)
            return AST.ExprStmt(call, name.context)
        elif first.value == '[':
            index = self.visit(node.children[1])  # Load
            value = self.visit(node.children[-1])  # Load
            subscript = AST.Subscript(name.value, index, store, node.context)
            return AST.Assign(subscript, value, name.context)
        else:
            assert first.value == '=', first
            value = self.visit(node.children[-1])  # Load
            target = AST.Name(name.value, store, name.context)
            return AST.Assign(target, value, name.context)

    def visit_if_stmt(self, node):
        condition, _, stmt, *trailer = node.children[2:]
        test = self.visit(condition)
        body = list(self.visit(stmt))
        orelse = list(self.visit(trailer[-1])) if trailer else []
        return AST.If(test, body, orelse, node.context)

    def visit_condition(self, node):
        return AST.BoolOp(self.visit_expr(node), node.context)

    def visit_for_stmt(self, node):
        # initial: stmt
        name, _, expr = node.children[2:5]
        initial = AST.Assign(
            AST.Name(name.value, expr_context.Store, name.context),
            self.visit(expr),
            name.context)

        condition = self.visit(node.children[6])

        # step: stmt
        target, _, name, op, num = node.children[8:13]
        name_ = AST.Name(name.value, expr_context.Load, name.context)
        op = AST.string2operator[op.value]
        # this is a NUMBER
        assert num.type == sym.NUMBER
        num = AST.Num(int(num.value), num.context)

        next = AST.BinOp(name_, op, num, name.context)
        step = AST.Assign(
            AST.Name(target.value, expr_context.Store, target.context),
            next, target.context)

        stmt = list(self.visit(node.children[-1]))
        return AST.For(initial, condition, step, stmt, node.context)

    def visit_while_stmt(self, node):
        condition = self.visit(node.children[2])
        stmt = list(self.visit(node.children[-1]))
        return AST.While(condition, stmt, node.context)

    def visit_return_stmt(self, node):
        _, *trailer = node.children
        if not trailer:
            return AST.Return(None, node.context)
        expr = self.visit(trailer[1])
        return AST.Return(expr, node.context)

    def visit_read_stmt(self, node):
        names = [AST.Name(c.value, expr_context.Store, node.context)
                 for c in node.children[2:-1:2]]
        return AST.Read(names, node.context)

    def visit_write_stmt(self, node):
        values = node.children[2:-1]
        expr = None
        string = None

        for val in values:
            if val.type == sym.expr:
                expr = self.visit(val)
            elif val.type == sym.STRING:
                string = AST.Str(val.value, val.context)
        return AST.Write(string, expr, node.context)

    def visit_expr(self, node, context=expr_context.Load):
        # be careful for the default Load!
        # handle all range of expression.

        if node.type in (sym.term, sym.expr, sym.condition):
            return self.visit_binop(node, context)
        else:
            # basic case: factor
            assert node.type in (sym.atom, sym.factor)
            return self.visit(node, context)

    def visit_binop(self, node, context):
        # this function must call visit_expr() instead of visit()!
        result = self.visit_expr(node.first_child, context)
        nops = (len(node.children) - 1) // 2

        for i in range(nops):
            next_oper = node.children[i * 2 + 1]
            op = AST.string2operator[next_oper.value]
            tmp = self.visit_expr(node.children[i * 2 + 2], context)
            tmp_result = AST.BinOp(result, op, tmp, next_oper.context)
            result = tmp_result
        return result

    def visit_factor(self, node, context):
        if len(node.children) == 1:
            # atom
            return self.visit(node.first_child, context)
        else:
            op = AST.string2unaryop[node.first_child.value]
            operand = self.visit(node.children[1], context)
            return AST.UnaryOp(op, operand, node.first_child.context)

    def visit_atom(self, node, context):
        first = node.first_child
        if first.type == sym.NAME:
            if len(node.children) == 1:
                # single name
                return AST.Name(first.value, context, first.context)
            # name with trailer: visit_trailer
            trailer = node.children[1]
            return self.visit(trailer, first.value, context)
        if first.type == sym.NUMBER:
            return AST.Num(int(first.value), first.context)
        if first.type == sym.CHAR:
            return AST.Char(first.value, first.context)
        else:
            assert first.value == '('
            # parenthesis expr
            # visit_expr
            value = self.visit(node.children[1], context)
            return AST.ParenExpr(value, first.context)

    def visit_atom_trailer(self, node, name, context):
        first = node.first_child
        if first.type == sym.arglist:
            args = self.visit(first)  # no empty arglist
            return AST.Call(name, args, node.context)
        else:
            assert first.value == '[', first
            index = self.visit(node.children[1], context)
            return AST.Subscript(name, index, context, node.context)

    def visit_arglist(self, node):
        # strip () and skip ,
        return [self.visit(c, expr_context.Load) for c in node.children[1:-1:2]]

    def visit_subscript2(self, node):
        return int(node.children[1].value)


def build_ast(cst_node):
    """Build an AST from cst_node"""
    return TransformerVisitor().visit(cst_node)
