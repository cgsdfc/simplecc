from functools import singledispatch
import AST
from symtable import Constant, Variable, Function, Array
from util import ErrorManager


def check_funcall(fn_type, fn_name, formal_args, e, loc):
    """fn_type is the Function type object
    fn_name is the name of the function being checked
    formal_args is a List[Tuple[basic_type, loc]]
    e is an ErrorManager
    """
    # check #args match first
    if len(formal_args) != len(fn_type.args):
        e.error("function {!r} expects {} arguments, got {}".format(
            fn_name, len(fn_type.args), len(formal_args)), loc)
        return
    for pos, (f_arg, (a_arg, a_loc)) in enumerate(zip(fn_type.args, formal_args), 1):
        if f_arg == a_arg:
            continue  # exact match
        if a_arg == basic_type.Void:
            e.error("cannot pass void-value argument to function {!r} at position {}".format(
                fn_name, pos), a_loc)


def check_readnames(names_locs, types, e):
    for (name, loc), type in zip(names_locs, types):
        if not isinstance(type, Variable):
            e.error("cannot use scanf() on type {}".format(type), loc)


def check_subscript(type, name, e):
    if not isinstance(type, Array):
        e.error("type {} doest not support subscription".format(type), name.loc)


class Result:

    def __init__(self, is_valid, type=None):
        self.is_valid = is_valid
        self.type = type


def checkProgram(node, symtable):
    ErrorManager e
    for decl in node.decls:
        if not isinstance(decl, FuncDef):
            continue
        checkFuncDef(decl, symtable.locals[decl.name], e)
    return e.is_ok()


def checkFuncDef(fun, local, e):
    for stmt in fun.stmts:
        checkStmt(stmt, local, e)


def checkStmt(stmt, local, e):
    if isinstance(stmt, Read):
        return checkRead(stmt, local, e)
    if isinstance(stmt, Write):
        return checkWrite(stmt, l
