"""Check syntax errors the parser unable to check"""

from AST import *
from itertools import chain
import sys

class VisitorBase(object):
    """Generic tree visitor for ASTs."""
    def __init__(self):
        self.cache = {}

    def visit(self, obj, *args):
        klass = obj.__class__
        meth = self.cache.get(klass)
        if meth is None:
            methname = "visit" + klass.__name__
            meth = getattr(self, methname)
            self.cache[klass] = meth
        return meth(obj, *args)


def error(msg, loc):
    print("{}:{}:error: {}".format(loc[0], loc[1], msg), file=sys.stderr)


class SyntaxValidator(VisitorBase):
    """Due to the limitation of grammar, some allowed constructs are ill-formed,
    like ``int main()``. This class discovers these constructs and accumulates
    errors, writing directly to stderr.
    """

    def __init__(self):
        super().__init__()

    def visit(self, node, *args):
        """Return True for absent visitor meth"""
        try:
            return super().visit(node, *args)
        except AttributeError:
            return True

    def visit_list(self, list_of_nodes):
        # check a list of node, nested list is not allowed
        return all(map(self.visit, list_of_nodes))

    def visitProgram(self, node):
        assert isinstance(node, Program)
        if len(node.decls) == 0:
            return True

        # visit children first
        child_ok = all(map(self.visit, node.decls))

        # check the order of declaration. it should be
        # [ConstDecl] [VarDecl] [FuncDef]
        decl_iter = iter(node.decls)
        decl = next(decl_iter)
        while isinstance(decl, ConstDecl):
            decl = next(decl_iter)
        while isinstance(decl, VarDecl):
            decl = next(decl_iter)
        while isinstance(decl, FuncDef):
            decl = next(decl_iter)
        try:
            decl = next(decl_iter)
        except StopIteration:
            ok = True
        else:
            ok = False
            error("unexpected {}".format(decl.__class__.__name__))

        last = self.node[-1]
        if not (isinstance(last, FuncDef) and last.name == 'main'):
            error("expected main()", last.loc)
            return False

        return child_ok and ok


    def visitConstDecl(self, node):
        # check that type and value match.
        assert isinstance(node, ConstDecl)
        if isinstance(node.type, basic_type.Int):
            if isinstance(node.value, Num):
                return True
            error("const int expects integer", node.value.loc)
            return False
        else:
            assert isinstance(node.type, basic_type.Character)
            if isinstance(node.value, Char):
                return True
            error("const char expects character", node.value.loc)
            return False


    def visitVarDecl(self, node):
        # check void variable and zero-sized array
        var_type = node.type
        if var_type.type == basic_type.Void:
            error("cannot declare void variable", node.loc)
            return False
        if var_type.is_array and var_type == 0:
            error("array size cannot be 0", node.loc)
            return False
        return True

    def visitarg(self, node):
        assert isinstance(node, arg)
        if node.type == basic_type.Void:
            error("cannot declare void argument", node.loc)
            return False
        return True

    def visitFuncDef(self, node):
        # check children
        # check void arg
        children = chain(node.args, node.decls, node.stmts)
        child_ok = self.visit_list(list(children))

        # check main
        if node.name == 'main':
            if node.return_type != basic_type.Void:
                error("main() must return void", node.loc)
                return False

        return child_ok

    # for stmts, assign is the major subject
    def visitAssign(self, node):
        # check that target is either name or subscript
        if not isinstance(left, (Name, Subscript)):
            error("only name or subscript can be assigned to")
            return False
        return True

    def visitFor(self, node):
        return self.visit_list(node.stmt)

    def visitWhile(self, node):
        return self.visit_list(node.body)

    def visitIf(self, node):
        return (self.visit_list(node.body) and self.visit_list(node.orelse))
