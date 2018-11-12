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
            try:
                meth = getattr(self, methname)
            except AttributeError:
                return True
            else:
                self.cache[klass] = meth
        return meth(obj, *args)


def error(msg, loc):
    print("Error in line {} column {}: {}".format(
        loc[0], loc[1], msg), file=sys.stderr)


class SyntaxValidator(VisitorBase):
    """Due to the limitation of grammar, some allowed constructs are ill-formed,
    like ``int main()``. This class discovers these constructs and accumulates
    errors, writing directly to stderr.
    """

    def __init__(self):
        super().__init__()

    def visit_list(self, list_of_nodes):
        # check a list of node, nested list is not allowed
        # force construction of a list
        return all(list(map(self.visit, list_of_nodes)))

    def visitProgram(self, node):
        assert isinstance(node, Program)
        if len(node.decls) == 0: # empty input has no main()
            error("expected main() at the end of input", (0, 0))
            return False

        # visit children first
        child_ok = self.visit_list(node.decls)

        # program order
        # check the order of declaration. it should be
        # [ConstDecl] [VarDecl] [FuncDef]
        decl_iter = iter(node.decls)
        decl = next(decl_iter)
        try:
            while isinstance(decl, ConstDecl):
                decl = next(decl_iter)
            while isinstance(decl, VarDecl):
                decl = next(decl_iter)
            while isinstance(decl, FuncDef):
                decl = next(decl_iter)
        except StopIteration:
            pass
        try:
            next(decl_iter)
        except StopIteration:
            ok = True
        else:
            msg = "unexpected {} {!r}".format(decl.__class__.__name__, decl.name)
            error(msg, decl.loc)
            ok = False

        # check the last declaration is the main function
        last = node.decls[-1]
        if not (isinstance(last, FuncDef) and last.name == 'main'):
            error("expected main() at the end of input", last.loc)
            return False

        return child_ok and ok


    def visitConstDecl(self, node):
        # check that type and value match.
        assert isinstance(node, ConstDecl)
        if node.type == basic_type.Int:
            if isinstance(node.value, Num):
                return True
            error("const int {!r} expects an integer".format(node.name),
                    node.value.loc)
            return False
        else:
            assert node.type == basic_type.Character
            if isinstance(node.value, Char):
                return True
            error("const char {!r} expects a character".format(node.name),
                    node.value.loc)
            return False


    def visitVarDecl(self, node):
        # check void variable and zero-sized array
        var_type = node.type
        if var_type.type == basic_type.Void:
            error("cannot declare {!r} as a void variable".format(node.name),
                    node.loc)
            return False
        if var_type.is_array and var_type.size == 0:
            error("array size of {!r} cannot be 0".format(node.name), node.loc)
            return False
        return True

    def visitarg(self, node, funcname):
        assert isinstance(node, arg)
        if node.type == basic_type.Void:
            msg ="cannot declare void argument {!r} of function {!r}".format(node.name, funcname)
            error(msg, node.loc)
            return False
        return True

    def visitFuncDef(self, node):
        # check void arg
        args_ok = all([self.visit(arg, node.name) for arg in node.args])

        # check children
        children = chain(node.decls, node.stmts)
        child_ok = self.visit_list(children)

        # check main
        if node.name == 'main':
            if node.return_type != basic_type.Void:
                error("main() must return void", node.loc)
                return False

        return args_ok and child_ok

    # for stmts, assign is the major subject
    def visitAssign(self, node):
        assert isinstance(node, Assign)
        # check that target is either name or subscript
        if not isinstance(node.target, (Name, Subscript)):
            error("only name or subscript can be assigned to", node.loc)
            return False
        if node.target.ctx != expr_context.Store:
            error("not in an assignment context", node.loc)
            return False
        return True

    def visitFor(self, node):
        return self.visit_list(node.body)

    def visitWhile(self, node):
        return self.visit_list(node.body)

    def visitIf(self, node):
        return (self.visit_list(node.body) and self.visit_list(node.orelse))


def validate(node):
    assert isinstance(node, AST)
    return SyntaxValidator().visit(node)
