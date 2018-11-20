# Automatically Generated File
from enum import Enum
from collections import namedtuple


class AST:
    '''Base class of AST'''

    def __repr__(self):
        return "{name}({data})".format(
            name=self.__class__.__name__,
            data=", ".join("{}={}".format(
                name,
                getattr(self, name)
            ) for name in self.__slots__)
        )

    @property
    def _fields(self):
        return self.__slots__

    def __iter__(self):
        for name in self.__slots__:
            child = getattr(self, name)
            if isinstance(child, list):
                yield from child
            else:
                yield child


class Program(AST):
    __slots__ = ("decls",)

    def __init__(self, decls):
        self.decls = decls


class decl(AST):
    pass


class ConstDecl(decl):
    __slots__ = ("type", "name", "value", "loc",)

    def __init__(self, type, name, value, loc):
        self.type = type
        self.name = name
        self.value = value
        self.loc = loc


class VarDecl(decl):
    __slots__ = ("type", "is_array", "size", "name", "loc",)

    def __init__(self, type, is_array, size, name, loc):
        self.type = type
        self.is_array = is_array
        self.size = size
        self.name = name
        self.loc = loc


class FuncDef(decl):
    __slots__ = ("return_type", "name", "args", "decls", "stmts", "loc",)

    def __init__(self, return_type, name, args, decls, stmts, loc):
        self.return_type = return_type
        self.name = name
        self.args = args
        self.decls = decls
        self.stmts = stmts
        self.loc = loc


arg = namedtuple("arg", "type name loc")


class stmt(AST):
    pass


class Read(stmt):
    __slots__ = ("names", "loc",)

    def __init__(self, names, loc):
        self.names = names
        self.loc = loc


class Write(stmt):
    __slots__ = ("str", "value", "loc",)

    def __init__(self, str, value, loc):
        self.str = str
        self.value = value
        self.loc = loc


class Assign(stmt):
    __slots__ = ("target", "value", "loc",)

    def __init__(self, target, value, loc):
        self.target = target
        self.value = value
        self.loc = loc


class For(stmt):
    __slots__ = ("initial", "condition", "step", "body", "loc",)

    def __init__(self, initial, condition, step, body, loc):
        self.initial = initial
        self.condition = condition
        self.step = step
        self.body = body
        self.loc = loc


class While(stmt):
    __slots__ = ("condition", "body", "loc",)

    def __init__(self, condition, body, loc):
        self.condition = condition
        self.body = body
        self.loc = loc


class Return(stmt):
    __slots__ = ("value", "loc",)

    def __init__(self, value, loc):
        self.value = value
        self.loc = loc


class If(stmt):
    __slots__ = ("test", "body", "orelse", "loc",)

    def __init__(self, test, body, orelse, loc):
        self.test = test
        self.body = body
        self.orelse = orelse
        self.loc = loc


class ExprStmt(stmt):
    __slots__ = ("value", "loc",)

    def __init__(self, value, loc):
        self.value = value
        self.loc = loc


class expr(AST):
    pass


class BinOp(expr):
    __slots__ = ("left", "op", "right", "loc",)

    def __init__(self, left, op, right, loc):
        self.left = left
        self.op = op
        self.right = right
        self.loc = loc


class UnaryOp(expr):
    __slots__ = ("op", "operand", "loc",)

    def __init__(self, op, operand, loc):
        self.op = op
        self.operand = operand
        self.loc = loc


class Call(expr):
    __slots__ = ("func", "args", "loc",)

    def __init__(self, func, args, loc):
        self.func = func
        self.args = args
        self.loc = loc


class Num(expr):
    __slots__ = ("n", "loc",)

    def __init__(self, n, loc):
        self.n = n
        self.loc = loc


class Str(expr):
    __slots__ = ("s", "loc",)

    def __init__(self, s, loc):
        self.s = s
        self.loc = loc


class Char(expr):
    __slots__ = ("c", "loc",)

    def __init__(self, c, loc):
        self.c = c
        self.loc = loc


class Subscript(expr):
    __slots__ = ("name", "index", "ctx", "loc",)

    def __init__(self, name, index, ctx, loc):
        self.name = name
        self.index = index
        self.ctx = ctx
        self.loc = loc


class Name(expr):
    __slots__ = ("id", "ctx", "loc",)

    def __init__(self, id, ctx, loc):
        self.id = id
        self.ctx = ctx
        self.loc = loc


operator = Enum("operator", "Add Sub Mult Div Eq NotEq Lt LtE Gt GtE")


unaryop = Enum("unaryop", "UAdd USub")


expr_context = Enum("expr_context", "Load Store")


basic_type = Enum("basic_type", "Int Character Void")


# Hard coded mapping from string to their ASTs
string2operator = {
    "+": operator.Add,
    "-": operator.Sub,
    "*": operator.Mult,
    "/": operator.Div,
    "==": operator.Eq,
    "!=": operator.NotEq,
    "<": operator.Lt,
    "<=": operator.LtE,
    ">": operator.Gt,
    ">=": operator.GtE,
}

string2unaryop = {
    "+": unaryop.UAdd,
    "-": unaryop.USub,
}

string2basic_type = {
    "int": basic_type.Int,
    "char": basic_type.Character,
    "void": basic_type.Void,
}
