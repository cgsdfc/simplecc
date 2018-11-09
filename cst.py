"""AST construction and validation"""

import AST
import sys
import Symbol as sym
from pprint import pprint

class VisitorBase:
    def __init__(self):
        self.cache = {}

    def visit(self, node, *args, **kwargs):
        assert node is not None
        symbol = sym.tok_name[node.type]
        meth = self.cache.get(symbol)
        if meth is None:
            methname = 'visit_' + symbol
            meth = getattr(self, methname, None)
            self.cache[symbol] = meth
        # don't silent the absent of meth
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
        decls = []
        for c in node.children[:-1]:
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
        return name.value, self.visit(konstant)

    def visit_konstant(self, node):
        first = node.first_child
        if first.type == sym.CHAR: # Character
            return AST.Char(first.value, node.context)
        assert first.type == sym.integer # integer
        return AST.Num(self.visit(first), node.first_child_context)

    def visit_integer(self, node):
        # extract optional sign and digits
        num = ''.join(map(lambda c: c.value, node.children))
        return int(num)


    def visit_declaration(self, node):
        type_name, name, trailer = node.children
        return list(self.visit(trailer, type_name, name))

    def visit_decl_trailer(self, node, type_name, name):
        # generator: need to be list()
        first = node.first_child
        type = self.visit(type_name)
        if first.value == ';': # a single non-array var_decl
            var_type = AST.VarType(type, False, 0)
            yield AST.VarDecl(var_type, name.value, type_name.context)
        elif first.value == '(': # a funcdef
            yield self.visit_funcdef(type_name, name.value, node.children)
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


    def visit_funcdef(self, type_name, name, children):
        return_type = self.visit(type_name)
        paralist = None if len(children) == 5 else children[1]
        args = [] if paralist is None else list(self.visit(paralist))
        compound_stmt = children[-2]
        decls, stmts = self.visit(compound_stmt)
        return AST.FuncDef(return_type, name, args, decls, stmts, type_name.context)

    def visit_paralist(self, node):
        child = iter(node.children)
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
        for c in node.children:
            visited = self.visit(c)
            if c.type in (sym.const_decl, sym.var_decl):
                decls.extend(list(visited))
            else:
                assert c.type == sym.stmt
                stmts.append(visited)
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
        if node.first_child.value == '(': # Call expr
            args = [] if len(node.children) == 2 else self.visit(node.children[1])
            call = AST.Call(name, args, node.context)
            return AST.Expr(call, node.context)
        # assign stmt
        return self.visit_assign(node, name)


    def visit_assign(self, node, name):
        store = AST.expr_context.Store
        if len(node.children) == 3:
            # subsript
            target = AST.Subscript(name, self.visit(node.children[0]), store, node.context)
        else:
            target = AST.Name(name, store, node.context)
        value = self.visit(node.children[-1])
        assert value
        return AST.Assign(target, value, node.context)


    def visit_if_stmt(self, node):
        condition, _, stmt, *trailer = node.children[2:]
        test = self.visit(condition)
        body = self.visit(stmt)
        orelse = self.visit(trailer[-1]) if trailer else None
        return AST.If(test, body, orelse, node.context)


    def visit_condition(self, node):
        return self.visit_expr(node)

    def visit_for_stmt(self, node):
        # initial: stmt
        name, _, expr = node.children[2:5]
        initial = AST.Assign(name.value, self.visit(expr, AST.expr_context.Store),
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
        step = AST.Assign(target.value, next, target.context)

        stmt = self.visit(node.children[-1])
        return AST.For(initial, condition, step, stmt, node.first_child_context)


    def visit_switch_stmt(self, node):
        expr = self.visit(node.children[2])
        *case_stmts, default = node.children[5:-1]
        default_stmt = self.visit(default)
        labels = list(map(self.visit, case_stmts))
        return AST.Switch(expr, labels, default_stmt, node.context)

    def visit_case_stmt(self, node):
        _, konstant, _, stmt = node.children
        return AST.label_stmt(self.visit(konstant), self.visit(stmt), node.context)

    def visit_default_stmt(self, node):
        return self.visit(node.children[-1])


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
        if first.value == '(':
            args = [] if len(node.children) == 2 else self.visit(node.children[1])
            return AST.Call(name, args, node.context)
        else:
            assert first.type == sym.subscript, first
            index = self.visit(first)
            return AST.Subscript(name, index, context, node.context)

    def visit_arglist(self, node):
        return [self.visit(c) for c in node.children[::2]]

    def visit_subscript2(self, node):
        return int(node.children[1].value)

    def visit_subscript(self, node):
        return self.visit(node.children[1])


def ToAST(node):
    return TransformerVisitor().visit(node)
