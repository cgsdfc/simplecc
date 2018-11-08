from enum import Enum
from collections import namedtuple

class AST:
    def __repr__(self):
        return "{name}({data})".format(name=self.__class__.__name__,
            data=", ".join("{}={}".format(name, getattr(self, name))
                for name in self.__slots__))

operator = Enum('operator', 'Add Sub Mult Div Eq NotEq Lt LtE Gt GtE')

unaryop = Enum('unaryop', 'UAdd USub')

expr_context = Enum('expr_context', 'Load Store')

type = Enum('type', 'Int Character Void Array')

class Program(AST):
    __slots__ = ('decls', 'functions')

    def __init__(self, decls, functions):
        self.decls = decls
        self.functions = functions


class ConstDecl(AST):
    __slots__ = ('name', 'decl_type', 'value', 'lineno', 'col_offset')

    def __init__(self, name, decl_type, value, lineno, col_offset):
        self.name = name
        self.decl_type = decl_type
        self.value = value
        self.lineno = lineno
        self.col_offset = col_offset


class VarDecl(AST):
    __slots__ = ('name', 'decl_type', 'lineno', 'col_offset')

    def __init__(self, name, decl_type, lineno, col_offset):
        self.name = name
        self.decl_type = decl_type
        self.lineno = lineno
        self.col_offset = col_offset


class FunctionDef(AST):
    __slots__ = ('name', 'args', 'return_type', 'declaration', 'body',
                 'lineno', 'col_offset')

    def __init__(self, name, args, return_type, declaration, body, lineno,
                 col_offset):
        self.name = name
        self.args = args
        self.return_type = return_type
        self.declaration = declaration
        self.body = body
        self.lineno = lineno
        self.col_offset = col_offset


arg = namedtuple('arg', 'type name lineno col_offset')

class Read(AST):
    __slots__ = ('names', 'lineno', 'col_offset')

    def __init__(self, names, lineno, col_offset):
        self.names = names
        self.lineno = lineno
        self.col_offset = col_offset


class Write(AST):
    __slots__ = ('str', 'value', 'lineno', 'col_offset')

    def __init__(self, str, value, lineno, col_offset):
        self.str = str
        self.value = value
        self.lineno = lineno
        self.col_offset = col_offset


class Assign(AST):
    __slots__ = ('target', 'value', 'lineno', 'col_offset')

    def __init__(self, target, value, lineno, col_offset):
        self.target = target
        self.value = value
        self.lineno = lineno
        self.col_offset = col_offset


class For(AST):
    __slots__ = ('counter', 'initial', 'condition', 'step', 'body', 'lineno',
                 'col_offset')

    def __init__(self, counter, initial, condition, step, body, lineno,
                 col_offset):
        self.counter = counter
        self.initial = initial
        self.condition = condition
        self.step = step
        self.body = body
        self.lineno = lineno
        self.col_offset = col_offset


class While(AST):
    __slots__ = ('condition', 'body', 'lineno', 'col_offset')

    def __init__(self, condition, body, lineno, col_offset):
        self.condition = condition
        self.body = body
        self.lineno = lineno
        self.col_offset = col_offset


class Switch(AST):
    __slots__ = ('value', 'labels', 'branches', 'lineno', 'col_offset')

    def __init__(self, value, labels, branches, lineno, col_offset):
        self.value = value
        self.labels = labels
        self.branches = branches
        self.lineno = lineno
        self.col_offset = col_offset


class Return(AST):
    __slots__ = ('value', 'lineno', 'col_offset')

    def __init__(self, value, lineno, col_offset):
        self.value = value
        self.lineno = lineno
        self.col_offset = col_offset


class If(AST):
    __slots__ = ('test', 'body', 'orelse', 'lineno', 'col_offset')

    def __init__(self, test, body, orelse, lineno, col_offset):
        self.test = test
        self.body = body
        self.orelse = orelse
        self.lineno = lineno
        self.col_offset = col_offset


class Expr(AST):
    __slots__ = ('value', 'lineno', 'col_offset')

    def __init__(self, value, lineno, col_offset):
        self.value = value
        self.lineno = lineno
        self.col_offset = col_offset


class BinOp(AST):
    __slots__ = ('left', 'op', 'right', 'lineno', 'col_offset')

    def __init__(self, left, op, right, lineno, col_offset):
        self.left = left
        self.op = op
        self.right = right
        self.lineno = lineno
        self.col_offset = col_offset


class UnaryOp(AST):
    __slots__ = ('op', 'operand', 'lineno', 'col_offset')

    def __init__(self, op, operand, lineno, col_offset):
        self.op = op
        self.operand = operand
        self.lineno = lineno
        self.col_offset = col_offset


class Call(AST):
    __slots__ = ('func', 'args', 'lineno', 'col_offset')

    def __init__(self, func, args, lineno, col_offset):
        self.func = func
        self.args = args
        self.lineno = lineno
        self.col_offset = col_offset


class Num(AST):
    __slots__ = ('n', 'lineno', 'col_offset')

    def __init__(self, n, lineno, col_offset):
        self.n = n
        self.lineno = lineno
        self.col_offset = col_offset


class Str(AST):
    __slots__ = ('s', 'lineno', 'col_offset')

    def __init__(self, s, lineno, col_offset):
        self.s = s
        self.lineno = lineno
        self.col_offset = col_offset


class Char(AST):
    __slots__ = ('c', 'lineno', 'col_offset')

    def __init__(self, c, lineno, col_offset):
        self.c = c
        self.lineno = lineno
        self.col_offset = col_offset


class Subscript(AST):
    __slots__ = ('value', 'slice', 'ctx', 'lineno', 'col_offset')

    def __init__(self, value, slice, ctx, lineno, col_offset):
        self.value = value
        self.slice = slice
        self.ctx = ctx
        self.lineno = lineno
        self.col_offset = col_offset


class Name(AST):
    __slots__ = ('id', 'ctx', 'lineno', 'col_offset')

    def __init__(self, id, ctx, lineno, col_offset):
        self.id = id
        self.ctx = ctx
        self.lineno = lineno
        self.col_offset = col_offset


