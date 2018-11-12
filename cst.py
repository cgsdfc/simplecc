"""Transform a CST to an AST"""

import AST
import sys
import Symbol as sym
from pprint import pprint

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
            # don't silent the absent of meth
            meth = getattr(self, methname)
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
        _, type_name, *const_items = node.children[:-1] # strip tailing semicolon
        type = self.visit(type_name)
        # visit_const_item
        name_vals = map(self.visit, const_items[::2]) # skip comma
        for name, val in name_vals:
            yield AST.ConstDecl(type, name, val, node.context) # type all the same


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
            return [AST.FuncDef(type, 'main', [], *compound_stmt, type_name.context)]
        else:
            assert trailer[-1].type == sym.decl_trailer
            decl_trailer = trailer[-1]
            return list(self.visit(decl_trailer, type_name, name))

    def visit_decl_trailer(self, node, type_name, name):
        # generator: need to be list()
        first = node.first_child
        type = self.visit(type_name)
        if first.value == ';': # a single non-array var_decl
            var_type = AST.VarType(type, False, 0)
            yield AST.VarDecl(var_type, name.value, type_name.context)
        elif first.type in (sym.paralist, sym.compound_stmt):
            yield self.visit_funcdef(type, name.value, node.children, type_name.context)
        else: # complex var_decl
            if first.type == sym.subscript2: # first item is an array
                var_items = node.children[1:-1] # strip subscript2 and semicolon
                # visit_subscript2
                var_type = AST.VarType(type, True, self.visit(first)) # type all the same
                yield AST.VarDecl(var_type, name.value, node.context)
            else: # first item is a basic_type
                var_items = node.children[:-1] # strip semicolon
                var_type = AST.VarType(type, False, 0)
                yield AST.VarDecl(var_type, name.value, node.context)

            # handle (',', var_items)*
            for name, is_array, size in map(self.visit, var_items[1::2]): # possibly leading comma
                # visit_var_item
                var_type = AST.VarType(type, is_array, size)
                yield AST.VarDecl(var_type, name, type_name.context)


    def visit_funcdef(self, return_type, name, children, context):
        # children is the children of decl_trailer
        # name is already an identifier
        paralist = [] if len(children) == 1 else list(self.visit(children[0]))
        compound_stmt = children[-1] # always the last one
        decls, stmts = self.visit(compound_stmt)
        return AST.FuncDef(return_type, name, paralist, decls, stmts, context)

    def visit_paralist(self, node):
        # strip leading ( and tailing )
        child = iter(node.children[1:-1])

        while True:
            type_name = next(child)
            type = self.visit(type_name)
            name = next(child).value
            yield AST.arg(type, name, type_name.context)
            next(child) # skip ','


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
        for name, is_array, size, in map(self.visit, var_items[::2]):
            var_type = AST.VarType(type, is_array, size)
            yield AST.VarDecl(var_type, name, type_name.context)


    def visit_var_item(self, node):
        name = node.first_child.value
        if len(node.children) == 1:
            # name, is_array, size
            return name, False, 0
        # visit_subscript2
        return name, True, self.visit(node.children[1])


    def visit_stmt(self, node):
        first = node.first_child
        if first.type == sym.flow_stmt:
            # flow_stmt is always one stmt
            yield self.visit(node.first_child.first_child)
        elif first.type == sym.expr:
            if len(node.children) == 2: # non Assign
                expr1 = self.visit(first) # Load by default
                yield AST.ExprStmt(expr1, node.context)
            else: # assign
                assert node.children[1].value == '='
                expr1 = self.visit(first, AST.expr_context.Store)
                expr2 = self.visit(node.children[-2])
                yield AST.Assign(expr1, expr2, node.context)
        elif first.value == '{':
            for child in node.children[1:-1]:
                yield from self.visit(child)
        else:
            # make sure we handle all cases.
            assert first.value == ';' and len(node.children) == 1, node

    def visit_if_stmt(self, node):
        condition, _, stmt, *trailer = node.children[2:]
        test = self.visit(condition)
        body = list(self.visit(stmt))
        orelse = list(self.visit(trailer[-1])) if trailer else []
        return AST.If(test, body, orelse, node.context)

    def visit_condition(self, node):
        return self.visit_expr(node)

    def visit_for_stmt(self, node):
        # initial: stmt
        name, _, expr = node.children[2:5]
        initial = AST.Assign(
                AST.Name(name.value, AST.expr_context.Store, name.context),
                self.visit(expr),
                name.context)

        # condition: expr
        condition = self.visit(node.children[6])

        # step: stmt
        target, _, name, op, num = node.children[8:13]
        name_ = AST.Name(name.value, AST.expr_context.Load, name.context)
        op = AST.string2operator[op.value]
        # this is a NUMBER
        assert num.type == sym.NUMBER
        num = AST.Num(int(num.value), num.context)

        next = AST.BinOp(name_, op, num, name.context)
        step = AST.Assign(
                AST.Name(target.value, AST.expr_context.Store, target.context),
                next, target.context)

        stmt = list(self.visit(node.children[-1]))
        return AST.For(initial, condition, step, stmt, node.first_child_context)

    def visit_while_stmt(self, node):
        condition = self.visit(node.children[2])
        stmt = list(self.visit(node.children[-1]))
        return AST.While(condition, stmt, node.context)

    def visit_return_stmt(self, node):
        _, *trailer = node.children
        if not trailer:
            return AST.Return(None, node.context)
        expr = self.visit(trailer[1])
        return AST.Return(expr, node.first_child_context)


    def visit_read_stmt(self, node):
        names = [c.value for c in node.children[2:-1:2]]
        return AST.Read(names, node.context)

    def visit_write_stmt(self, node):
        values = node.children[2:-1]
        expr = None
        string = None

        for val in values:
            if val.type == sym.expr:
                expr = self.visit(val)
            elif val.type == sym.STRING:
                string = val.value
        return AST.Write(string, expr, node.first_child_context)


    def visit_expr(self, node, context=AST.expr_context.Load):
        # handle all range of expression.
        unaryop = None
        # optional unaryop in expr
        if node.type == sym.expr:
            if node.first_child.value in ('-', '+'):
                unaryop = AST.string2unaryop[node.first_child.value]
                node.children = node.children[1:]

        if node.type in (sym.term, sym.expr, sym.condition):
            result = self.visit_binop(node, context)
        else:
            # basic case: factor
            assert node.type == sym.factor
            result = self.visit(node, context)
        if unaryop:
            return AST.UnaryOp(unaryop, result, node.context)
        return result


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
        # STRING is not a kind of factor
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
            # visit_expr
            return self.visit(node.children[1])

    def visit_factor_trailer(self, node, name, context):
        first = node.first_child
        if first.type == sym.arglist:
            args = self.visit(first) # no empty arglist
            return AST.Call(name, args, node.context)
        else:
            assert first.value == '[', first
            index = self.visit(node.children[1])
            return AST.Subscript(name, index, context, node.context)

    def visit_arglist(self, node):
        # strip () and skip ,
        return [self.visit(c) for c in node.children[1:-1:2]]

    def visit_subscript2(self, node):
        return int(node.children[1].value)


def ToAST(node):
    return TransformerVisitor().visit(node)


def astpretty_pprint(rootnode):
    import astpretty
    import AST

    astpretty.customize(AST.AST, AST.expr_context)
    astpretty.pprint(rootnode)
