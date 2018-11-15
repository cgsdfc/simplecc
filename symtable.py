from functools import singledispatch

from AST import *

class Entry:

    def __init__(self, key, name):
        self.key = key
        self.symbols = {}
        self.name = name
        self.type = None

class SymbolTable:

    def __init__(self):
        # current active Entry
        self.current_entry = None
        self.stack = []
        self.blocks = {}
        self.top = None

    def enter_scope(self, node, ):
        self.stack.append(self.current_entry)
        self.current_entry = STEntry(

    def exit_scope(self):
        pass

    def visitProgram(self, node):
        pass


@singledispatch
def visit(x):
    pass

@visit.register(Program)
def _(x):

