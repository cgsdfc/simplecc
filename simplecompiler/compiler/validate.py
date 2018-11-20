"""Check syntax errors the parser unable to check"""

import sys
from itertools import chain
from functools import singledispatch

from simplecompiler.compiler.AST import *
from simplecompiler.util import error


@singledispatch
def visit(node):
    return True


@visit.register(list)
def _(list_of_nodes):
    # check a list of node, nested list is not allowed
    # force construction of a list
    return all(list(map(visit, list_of_nodes)))


@visit.register(Program)
def _(node):
    assert isinstance(node, Program)
    if len(node.decls) == 0:  # empty input has no main()
        error("expected main() at the end of input", (0, 0))
        return False

    # visit children first
    child_ok = visit(node.decls)

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


@visit.register(ConstDecl)
def _(node):
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


@visit.register(VarDecl)
def _(node):
    # check void variable and zero-sized array
    if node.type == basic_type.Void:
        error("cannot declare {!r} as a void variable".format(node.name),
              node.loc)
        return False
    if node.is_array and node.size == 0:
        error("array size of {!r} cannot be 0".format(node.name), node.loc)
        return False
    return True


@visit.register(arg)
def _(node, funcname):
    assert isinstance(node, arg)
    if node.type == basic_type.Void:
        msg = "cannot declare void argument {!r} of function {!r}".format(
            node.name, funcname)
        error(msg, node.loc)
        return False
    return True


@visit.register(FuncDef)
def _(node):
    # check void arg
    args_ok = all([visit(arg, node.name) for arg in node.args])

    # check children
    children = chain(node.decls, node.stmts)
    child_ok = visit(children)

    # check main
    if node.name == 'main':
        if node.return_type != basic_type.Void:
            error("main() must return void", node.loc)
            return False

    return args_ok and child_ok


def validate(node):
    assert isinstance(node, AST)
    return visit(node)
