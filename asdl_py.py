#! /usr/bin/env python3

"""Ast generation"""
import asdl
import util
from operator import attrgetter


def is_simple(sum):
    """Return True if a sum is a simple.

    A sum is simple if its types have no fields, e.g.
    unaryop = Invert | Not | UAdd | USub
    """
    return not any(t.fields for t in sum.types)


def get_args(fields, attrs):
    """Return a list of names from fields and attrs"""
    # field name is optional
    from itertools import chain
    args = []
    unnamed = {}
    for f in chain(fields, attrs):
        if f.name is None:
            name = f.type
            c = unnamed[name] = unnamed.get(name, 0) + 1
            if c > 1:
                name = "name%d" % (c - 1)
        else:
            name = f.name
        args.append(name)
    return args


class AstEmittor(util.Emittor, asdl.VisitorBase):
    """Base class for Both an Emittor and a Visitor"""

    def __init__(self, file):
        super().__init__(file)

    def visitModule(self, mod):
        for dfn in mod.dfns:
            self.visit(dfn)


class EnumEmittor(AstEmittor):
    """Emit enum for simple_sum"""

    def visitType(self, type):
        if not isinstance(type.value, asdl.Sum):
            return
        sum = type.value
        if not is_simple(sum):
            return
        self.emit("{name} = Enum({name!r}, {args!r})".format(
            name=type.name, args=" ".join(map(attrgetter('name'), sum.types))))
        self.emit("")


class ClassEmittor(AstEmittor):
    """Emit class for Constructor"""

    def visitType(self, type):
        if isinstance(type.value, asdl.Product):
            self.visitProduct(type.value, type.name)
        elif isinstance(type.value, asdl.Sum):
            self.visitSum(type.value)


    def visitProduct(self, prod, name):
        self.emit("{name} = namedtuple({name!r}, {fields!r})".format(
            name=name, fields=" ".join(get_args(prod.fields, prod.attributes))))
        self.emit("")

    def visitSum(self, sum):
        if is_simple(sum):
            return
        for type in sum.types:
            self.visitConstructor(type, sum.attributes)
            self.emit("")

    def visitConstructor(self, cons, attrs):
        args = get_args(cons.fields, attrs)
        self.emit("class {name}(AST):".format(name=cons.name))
        self.emit("__slots__ = ({attrs})".format(attrs=", ".join(map(repr, args))), 1)
        self.emit("")
        self.emit("def __init__(self, {args}):".format(
            args=", ".join(args)), 1)
        for arg in args:
            self.emit("self.{attr} = {attr}".format(attr=arg), 2)
        self.emit("")


Header = """from enum import Enum
from collections import namedtuple

class AST:
    def __repr__(self):
        return "{name}({data})".format(name=self.__class__.__name__,
            data=", ".join("{}={}".format(name, getattr(self, name))
                for name in self.__slots__))

"""


def main():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('input', type=str, help='path/to/asdl')
    parser.add_argument('-o', '--output', dest='output',
        type=argparse.FileType('w'), default=sys.stdout)
    parser.add_argument('--dump_module', action='store_true', default=False,
            help='Dump the asdl module and exit')
    args = parser.parse_args()

    mod = asdl.parse(args.input)
    if args.dump_module:
        print(mod)
        return 0
    if not asdl.check(mod):
        return 1

    with args.output as f:
        f.write(Header)
        c = util.ChainOfVisitors(EnumEmittor(f), ClassEmittor(f))
        c.visit(mod)
    return 0

if __name__ == '__main__':
    import sys
    sys.exit(main())