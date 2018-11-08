"""AST construction and validation"""

import AST
import sys


class CSTVisitorBase:
    def __init__(self, grammar):
        self.grammar = grammar
        self.cache = {}

    def visit(self, node, *args, **kwargs):
        type = node[0]
        symbol = self.grammar.number2symbol[type]
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
        assert node[0] == self.program
        const_decl = []
        declaration = []

        for child in node[-1]:
            if child[0] == self.const_decl:
                const_decl.extend(self.visit(child))
            elif child[0] == self.declaration:
                declaration.append(self.visit(child))
            else:
                assert child[0] == self.ENDMARKER
        return AST.Program(decls=const_decl + declaration)

    def visit_const_decl(self, node):
        children = node[-1]
        typename = children[1]
        decl_type = self.visit(typename)
        # skip comma
        name_vals = map(self.visit, children[2:-1:2])

        for name, val in name_vals:
            yield AST.ConstDecl(name=name, decl_type=decl_type, value=val)

    def visit_const_item(self, node):
        children = node[-1]
        name = children[0][0]
        value = self.visit(children[1])
        return name, value

    def visit_konstant(self, node):
        if node[0] == self.CHAR:
            return AST.Char(node[1], *node[2])
        assert node[0] == self.integer
        return self.visit(node)

    def visit_integer(self, node):
        children = node[-1]
        sign, abs = children
        digits = sign[1] + abs[1]
        return AST.Num(int(digits), *node[2])

    def visit_declaration(self, node):
        typename, name, trailer = node[-1]
        prefix = trailer[-1][1]
        if prefix == '(':
            # funcdef
        elif prefix == '[':
            # index = self.visit(trailer

    def visit_funcdef(self, node):
        typename, name, _, *args, _, _, compound_stmt, *_ = node[-1]
        name = name[1]
        assert isinstance(name, str)

        def visit_arg(node):
            return self.visit(node[-1][0]), node[-1][1][1]
        args = list(map(visit_arg, args))

        return_type = self.visit(typename)
        declaration, *stmt, self.visit(compound_stmt)
        return AST.FunctionDef(name=name, args=args,
                return_type=return_type, body=stmt)

    def visit_type_name(self, node):
        name = node[-1][1]
        if name == 'int':
            return AST.type.Int
        if name == 'char':
            return AST.type.Character
        assert name == 'void'
        return AST.type.Void







