"""AST construction and validation"""

import AST
import sys
import Symbol as sym

class CSTVisitorBase:
    def __init__(self, grammar):
        self.grammar = grammar
        self.cache = {}

    def visit(self, node, *args, **kwargs):
        type = node[0]
        symbol = sym.tok_name[type]
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


class ConstructionVisitor(CSTVisitorBase):
    """Visitor transforming CST to AST"""

    def visit_program(self, node):
        assert node.type == sym.program
        const_decl = []
        declaration = []

        for child in node.children:
            if child[0] == self.const_decl:
                const_decl.extend(self.visit(child))
            elif child[0] == self.declaration:
                declaration.append(self.visit(child))
            else:
                assert child[0] == ENDMARKER
        return AST.Program(decls=const_decl + declaration)

    def visit_const_decl(self, node):
        _, type_name, *const_items, _ = node.children
        type = self.visit(type_name)
        # skip comma
        name_vals = map(self.visit, const_items[0:-1:2])
        for name, val in name_vals:
            yield AST.ConstDecl(type=type, name=name, value=val)


    def visit_const_item(self, node):
        name, _, konstant = node.children
        return self.visit(name), self.visit(konstant)

    def visit_NAME(self, node):
        return node.value


    def visit_konstant(self, node):
        if node.type == sym.CHAR:
            return AST.Char(node.children[0].value, *node.context)
       assert node.type == sym.integer
       # use context of sign
       return AST.Num(self.visit(node), *node.first_child_context)


    def visit_declaration(self, node):
        type_name, name, trailer = node.children
        prefix = trailer.children[0]
        if prefix.value == '(':
            return self.visit_funcdef(type_name, name, trailer)
        # TODO:


    def visit_funcdef(self, type_name, name, trailer):
        paralist = None if len(trailer) == 5 else trailer[1]
        compound_stmt = trailer[-2]
        args = self.visit(paralist) if paralist else []
        return_type = self.visit(type_name)
        decls, stmts = self.visit(compound_stmt)
        return AST.FunctionDef(return_type, name, args, decls, stmts, *node.context)


    def visit_type_name(self, node):
        name = node.children[0].value
        if name == 'int':
            return AST.type.Int
        if name == 'char':
            return AST.type.Character
        assert name == 'void'
        return AST.type.Void

    def visit_var_decl(self, node):
        type_name, *var_items, *_ = node.children
        type = self.visit(type_name)
        for name, args in map(self.visit_var_item, var_items[0:-1:2]):
            var_type = AST.VarType(type, *args)
            yield AST.VarDecl(var_type, name)


    def visit_var_item(self, node):
        name = node.children[0].value
        if len(node.children) == 1:
            # name, is_array, size
            return name, None, None
        assert len(node.children) == 3, node
        return name, True, int(node.children[-2].value)


    def visit_stmt(self, node):
        prefix = node.children[0].type
        if prefix >= 256: # if_stmt or so
            assert len(node.children) == 1
            return self.visit(node.children[0])
        if prefix == sym.NAME: # expr_stmt
            pass
        value = node.children[0].value
        if value == '{':
            # skip { and }
            # filter pure ';'
            return list(filter(None, map(self.visit, node.children[1:-1])))
        assert value == ';'
        return None


    def visit_if_stmt(self, node):
        _, _, condition, _, stmt, *trailer = node.children
        expr = self.visit(condition)
        body = self.visit(stmt)
        orelse = self.visit(trailer[-1]) if trailer else None
        return AST.If(expr, body, orelse, *node.context)


    def visit_condition(self, node):
        left, *trailer = node.children
        left = self.visit(left)
        if not trailer:
            return left
        assert len(trailer) == 2, trailer
        op = sym.string2operator[trailer[1].value]
        right = self.visit(trailer[-1])
        return AST.BinOp(left, op, right, *node.context)


    def visit_for_stmt(self, node):
        name, _, expr = node.children[2:5]
        initial = AST.Assign(name.value, self.visit(expr), *name.context)
        condition = self.visit(node.children[6])
        stmt = self.visit(node.children[-1])
        target, _, name, op, num = node.children[8:13]
        assert target.type == sym.NAME, target
        assert name.value in ('+', '-'), name
        step = AST.Assign(target.value,
                AST.BinOp(AST.Name(name, AST.expr_context.Load, *name.context),
                    sym.string2operator[op.value],
                    AST.Num(int(num.value), *num.context), *name.context),
                *target.context)
        return AST.For(initial, condition, step, stmt, *node.first_child_context)


    def visit_case_stmt(self, node):
        _, konstant, _, stmt = node.children
        return AST.label_stmt(self.visit(konstant), self.visit(stmt),
                *node.first_child_context)


    def visit_switch_stmt(self, node):
        expr = node.children[2]
        *case_stmts, default = node.children[5:-1]
        default_stmt = self.visit(default)

        labels = list(map(self.visit, case_stmts.children))
        return AST.Switch(expr, labels, default_stmt, *node.context)


    def visit_return_stmt(self, node):
        _, *trailer = node.children
        if len(trailer) == 1: # bare ;
            return AST.Return(None, *node.first_child_context)
        expr = self.visit(trailer[1])
        return AST.Return(expr, *node.first_child_context)


    def visit_read_stmt(self, node):
        names = [c.name for c in node.children[2:-1:2]]
        return AST.Read(names, *node.first_child_context)

    def visit_write_stmt(self, node):
        values = node.children[3:-1]
        expr = None
        string = None

        assert len(values) in range(1, 3)
        for val in values:
            if val.type == sym.expr:
                expr = self.visit(val)
            else:
                assert val.type == sym.STRING
                string = eval(val.value)
        return AST.Write(string, value, node.first_child_context)

    def visit_compound_stmt(self, node):
        decls = []
        stmts = []
        for c in node.children:
            visited = self.visit(c)
            if c.type == sym.const_decl:
                decls.append(visited)
            else:
                assert c.type == sym.stmt
                stmts.append(visited)
        return decls, stmts

    def visit_arglist(self, node):
        return [self.visit(c) for c in node.children[0:-1:2]]

    def visit_default_stmt(self, node):
        return self.visit(node.children[-1])

    def visit_paralist(self, node):
        for type_name, name in node.children[0:-1:2]:
            type = self.visit(type_name)
            name = name.value
            yield AST.arg(type, name)

    def visit_subscript2(self, node):
        return int(node.children[1].value)

    def visit_subscript(self, node):
        return self.visit(node.children[1])

    def visit_integer(self, node):
       # extract sign and digits, both tokens
       num = ''.join(map(lambda c: c[0].value, node.children))
       return int(num)







