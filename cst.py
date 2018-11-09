"""AST construction and validation"""

import AST
import sys
import Symbol as sym

class VisitorBase:
    def __init__(self):
        self.cache = {}

    def visit(self, node, *args, **kwargs):
        symbol = sym.tok_name[node.type]
        meth = self.cache.get(symbol)
        if meth is None:
            methname = 'visit_' + symbol
            meth = getattr(self, methname, None)
            self.cache[symbol] = meth
        if meth:
            try:
                return meth(node, *args, **kwargs)
            except Exception as e:
                sys.stderr.write("Error visiting {symbol!r}: {error}\n".format(
                    symbol=symbol, error=e))
                raise


class TransformerVisitor(VisitorBase):
    """Visitor transforming CST to AST"""

    def visit_program(self, node):
        assert node.type == sym.program
        const_decl = []
        declaration = []

        for child in node.children:
            if child.type == sym.const_decl:
                const_decl.extend(self.visit(child))
            elif child.type == sym.declaration:
                declaration.append(self.visit(child))
            else:
                assert child.type == sym.ENDMARKER

        return AST.Program(decls=const_decl + declaration)

    def visit_const_decl(self, node):
        _, type_name, *const_items = node.children[:-1]
        type = self.visit(type_name)
        # skip comma
        name_vals = map(self.visit, const_items[::2])
        for name, val in name_vals:
            yield AST.ConstDecl(type=type, name=name, value=val)


    def visit_const_item(self, node):
        name, _, konstant = node.children
        return name.value, self.visit(konstant)

    def visit_konstant(self, node):
        if node.type == sym.CHAR:
            return AST.Char(node.first_child.value, *node.context)
        assert node.type == sym.integer
        return AST.Num(self.visit(node), *node.first_child_context)

    def visit_integer(self, node):
       # extract sign and digits, both tokens
       num = ''.join(map(lambda c: c[0].value, node.children))
       return int(num)


    def visit_declaration(self, node):
        type_name, name, trailer = node.children
        first = trailer.first_child
        if first.value == '(':
            return self.visit_funcdef(type_name, name, trailer)
        else: # var_decl
            return self.visit_var_decl(node)


    def visit_funcdef(self, type_name, name, trailer):
        paralist = None if len(trailer) == 5 else trailer[1]
        compound_stmt = trailer[-2]
        args = list(self.visit(paralist))
        return_type = self.visit(type_name)
        decls, stmts = self.visit(compound_stmt)
        return AST.FunctionDef(return_type, name, args, decls, stmts, *node.context)

    def visit_paralist(self, node):
        if node is None:
            return
        child = iter(node.children)
        while True:
            type = self.visit(next(child))
            name = next(child).value
            yield AST.arg(type, name)
            next(child) # skip ','


    def visit_type_name(self, node):
        name = node.first_child.value
        return AST.string2basic_type[name]

    def visit_compound_stmt(self, node):
        decls = []
        stmts = []
        for c in node.children:
            visited = self.visit(c)
            if c.type in (sym.const_decl, sym.var_decl):
                decls.append(visited)
            else:
                assert c.type == sym.stmt
                stmts.append(visited)
        return decls, stmts


    def visit_var_decl(self, node):
        type_name, *var_items = node.children[:-1]
        type = self.visit(type_name)
        for name, args in map(self.visit, var_items[::2]):
            var_type = AST.VarType(type, *args)
            yield AST.VarDecl(var_type, name)


    def visit_var_item(self, node):
        name = node.first_child.value
        if len(node.children) == 1:
            # name, is_array, size
            return name, None, None
        # visit_subscript2
        return name, True, self.visit(node.children[1])


    def visit_stmt(self, node):
        first = node.first_child
        if first.type >= 256: # if_stmt
            # strip trailing semicolon
            return self.visit(node.first_child)
        if first.type == sym.NAME: # expr_stmt
            trailer = node.children[1]
            return self.visit(trailer, first.value)
        if first.value == '{':
            return list(filter(None, map(self.visit, node.children[1:-1])))
        # make sure we handle all cases.
        assert first.value == ';' and len(node.children) == 1, node
        return None

    def visit_stmt_trailer(self, node, name):
        if node.first_child.value == '(':
            args = [] if len(node.children) == 2 else self.visit(node.children[1])
            call = AST.Call(name, args)
            return AST.Expr(call, *node.context)
        return visit_assign(self, node, name)


    def visit_assign(self, node, name):
        store = AST.expr_context.Store
        if len(node.children) == 3:
            # subsript
            target = AST.Subscript(name, self.visit(node.children[0]), store, *node.context)
        else:
            target = AST.Name(name, store, *node.context)
        return AST.Assign(expr, self.visit(node.children[-1]), *node.context)


    def visit_if_stmt(self, node):
        condition, _, stmt, *trailer = node.children[2:]
        test = self.visit(condition)
        body = self.visit(stmt)
        orelse = self.visit(trailer[-1]) if trailer else None
        return AST.If(test, body, orelse, *node.context)


    def visit_condition(self, node):
        if len(node.children) == 1:
            return self.visit(node.first_child)
        assert len(node.children) == 3
        return AST.BinOp(self.visit(node.first_child),
                AST.string2operator[node.children[1].value],
                self.visit(node.children[-1]))


    def visit_for_stmt(self, node):
        # initial: stmt
        name, _, expr = node.children[2:5]
        initial = AST.Assign(name.value, self.visit(expr, AST.expr_context.Store),
                *name.context)

        # condition: expr
        condition = self.visit(node.children[6])

        # step: stmt
        target, _, name, op, num = node.children[8:13]
        name = AST.Name(name, AST.expr_context.Load, *name.context)
        op = AST.string2operator[op.value]
        num = AST.Num(int(num.value), *num.context)
        expr = AST.BinOp(name, op, num)
        target = target.value
        step = AST.Assign(target, expr)

        stmt = self.visit(node.children[-1])
        return AST.For(initial, condition, step, stmt, *node.first_child_context)


    def visit_switch_stmt(self, node):
        expr = node.children[2]
        *case_stmts, default = node.children[5:-1]
        default_stmt = self.visit(default)
        labels = list(map(self.visit, case_stmts))
        return AST.Switch(expr, labels, default_stmt, *node.context)

    def visit_case_stmt(self, node):
        _, konstant, _, stmt = node.children
        return AST.label_stmt(self.visit(konstant), self.visit(stmt), *node.context)

    def visit_default_stmt(self, node):
        return self.visit(node.children[-1])


    def visit_return_stmt(self, node):
        _, *trailer = node.children
        if not trailer
            return AST.Return(None, *node.context)
        expr = self.visit(trailer[1])
        return AST.Return(expr, *node.first_child_context)


    def visit_read_stmt(self, node):
        names = [c.name for c in node.children[2:-1:2]]
        return AST.Read(names, *node.context)

    def visit_write_stmt(self, node):
        values = node.children[2:-1]
        expr = None
        string = None

        for val in values:
            if val.type == sym.expr:
                expr = self.visit(val)
            elif val.type == sym.STRING
                string = eval(val.value)
        return AST.Write(string, expr, node.first_child_context)


    def visit_expr(self, node, context=None):
        # handle all range of expression.
        if node.type == sym.expr and node.first_child.value in '-+':
            # optional unaryop in expr
            unaryop = AST.string2unaryop[node.first_child.value]
            node.children = node.children[1:]
        else:
            unaryop = None

        if node.type in (sym.term, sym.expr):
            result = self.visit_binop(node)
        else:
            assert node.type == sym.factor
            result = self.visit(node, context)
        if unaryop:
            return AST.UnaryOp(unaryop, result, *node.context)
        return result


    def visit_binop(self, node):
        result = self.visit(node.first_child)
        nops = (len(node.children) - 1) / 2
        for i in range(nops):
            next_oper = node.children[i * 2 + 1]
            newoperator = AST.string2operator[next_oper.value]
            tmp = self.visit_expr(node.children[i * 2 + 2])
            tmp_result = AST.BinOp(result, newoperator, tmp, *next_oper.context)
            result = tmp_result
        return result


    def visit_factor(self, node, context):
        first = node.first_child
        if first.type == sym.NAME:
            if len(node.children) == 1:
                return AST.Name(first.value, context, *first.context)
            trailer = node.children[1]
            return self.visit(trailer, first.value, context)
        if first.value == '(':
            return self.visit(node.children[1])
        if first.type == sym.STRING:
            return AST.Str(eval(first.value), *first.context)
        if first.type == sym.NUMBER:
            return AST.Num(int(first.value), *first.context)
        if first.type == sym.CHAR:
            return AST.Char(eval(first.value), *first.context)

    def visit_trailer(self, node, name, context):
        first = node.first_child
        if first == '[':
            index = self.visit(node.children[1])
            return AST.Subscript(name, index, context, *node.context)
        assert first == '(', first
        args = [] if len(node.children) == 2 else self.visit(node.children[1])
        return AST.Call(name, args)

    def visit_arglist(self, node):
        return [self.visit(c) for c in node.children[::2]]

    def visit_subscript2(self, node):
        return int(node.children[1].value)

    def visit_subscript(self, node):
        return self.visit(node.children[1])


def ToAST(node):
    return TransformerVisitor().visit(node)
