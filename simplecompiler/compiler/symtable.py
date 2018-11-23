"""Symbol table construction"""

from functools import singledispatch
from collections import namedtuple
from enum import Enum
from operator import itemgetter
from operator import attrgetter
from pprint import pprint

# For generating tests
from string import Template
import re

from simplecompiler.compiler.AST import *
from simplecompiler.util import ErrorManager
from simplecompiler.util import format_code
Arg = arg

__all__ = ["build_symtable", "SymbolTable", "symtable_make_test"]


# An Entry represents a single resolved symbol
Entry = namedtuple("Entry", "type scope loc name")
Scope = Enum("Scope", "Local Global")
MODULE_NAME = "<module>"


class Type:
    def __repr__(self):
        return self.__class__.__name__

# pure data class to hold type information


class Constant(Type):

    def __init__(self, type):
        self.type = type


class Variable(Type):

    def __init__(self, type):
        self.type = type


class Array(Type):

    def __init__(self, elemtype, size):
        self.elemtype = elemtype
        self.size = size


class Function(Type):

    def __init__(self, return_type, args):
        self.return_type = return_type
        self.args = args

# Convert type declaration to Types


@singledispatch
def decl_to_type(decl):
    raise TypeError("unknown decl {}".format(decl.__class__.__name__))


@decl_to_type.register(ConstDecl)
def ConstDecl2Type(decl):
    return Constant(decl.type)


@decl_to_type.register(VarDecl)
def VarDecl2Type(decl):
    if decl.is_array:
        return Array(decl.type, decl.size)
    else:
        return Variable(decl.type)


@decl_to_type.register(FuncDef)
def FuncDef2Type(decl):
    # discard names
    args = list(map(itemgetter(0), decl.args))
    return Function(decl.return_type, args)


def define_decl(decl, scope, dict, e, where):
    if decl.name in dict:
        msg = "redefinition of identifier {!r} in {}".format(decl.name, where)
        e.error(msg, decl.loc)
    else:
        type = decl_to_type(decl)
        dict[decl.name] = Entry(type, scope, decl.loc, decl.name)


def define_arg(arg, dict, e, funcname):
    if arg.name in dict:
        msg = "redefinition of argument {!r}".format(arg.name)
        e.error(msg, arg.loc)
    else:
        type = Variable(arg.type)
        dict[arg.name] = Entry(type, Scope.Local, arg.loc, arg.name)


def make_global(prog, e):
    top = {}
    for decl in prog.decls:
        if isinstance(decl, FuncDef):
            continue
        define_decl(decl, Scope.Global, top, e, MODULE_NAME)
    return top


class LocalResolver:

    def __init__(self, fun, top, local, e):
        self.fun = fun
        self.top = top
        self.local = local
        self.e = e

    def resolve_name(self, name, loc):
        if name in self.local:
            return
        if name in self.top:
            type, scope, _, name = self.top[name]
            self.local[name] = Entry(type, scope, loc, name)
        else:
            msg = "undefined identifier {!r} in function {!r}".format(
                    name, self.fun.name)
            self.e.error(msg, loc)

    def resolve(self):
        for name, loc in iter_names(self.fun):
            self.resolve_name(name, loc)



def make_local(fun, top, e):
    local = {}
    define_decl(fun, Scope.Global, top, e, MODULE_NAME)

    # insert all arguments first
    for arg in fun.args:
        define_arg(arg, local, e, fun.name)

    # then local const/var decl
    where = "function {!r}".format(fun.name)
    for decl in fun.decls:
        define_decl(decl, Scope.Local, local, e, where)

    LocalResolver(fun, top, local, e).resolve()
    return local


def iter_names(node):
    # XXX: this must cover all Ast node that has names
    # handle terminal nodes that has a name:
    if isinstance(node, (ConstDecl, VarDecl, Arg)):
        yield node.name, node.loc
    elif isinstance(node, (Read,)):
        for name in node.names:
            yield name.id, node.loc
    elif isinstance(node, Call):
        yield node.func, node.loc
        for arg in node.args:
            yield from iter_names(arg)
    elif isinstance(node, Name):
        yield node.id, node.loc
    elif isinstance(node, Subscript):
        yield node.name, node.loc
    else:
        # handle non-terminal nodes
        for child in node:
            if isinstance(child, AST):
                yield from iter_names(child)


class SymbolTable:
    """All-in-one container of all names and their information within a program.
    """

    def __init__(self, global_, locals_):
        self.global_ = global_
        self.locals_ = locals_

    def report(self, output):
        print("Global:", file=output)
        global_ = sorted(self.global_.values(), key=attrgetter('loc'))
        pprint(global_, stream=output)
        print(file=output)  # make a blank line
        print("Locals:", file=output)
        pprint(self.locals_, stream=output)


def build_symtable(prog):
    """Build a SymbolTable for the whole program"""
    assert isinstance(prog, Program)
    e = ErrorManager()
    top = make_global(prog, e)
    locals_ = {}

    for decl in prog.decls:
        if isinstance(decl, FuncDef):
            locals_[decl.name] = make_local(decl, top, e)

    return SymbolTable(top, locals_) if e.is_ok else None


def pyenum2cppenum(enum):
    # replace dot with ::
    return re.sub(r"\.", "::", str(enum))


class CppTestTempalte:
    impl = Template("""
#include "symtable.h"
#include <cassert>

void TestSymbolTable(SymbolTable& symtable) {
    auto &global = symtable.global;
    auto &locals = symtable.locals;
$test_global
$test_locals
}
""")

    testEntry = Template("""
{
assert($table.count("$name"));
const SymbolEntry &e = $table.at("$name");
assert(e.Is$class_name());
assert(e.GetScope() == $scope);
assert(e.GetName() == "$name");
}
""")

    testGlobal = Template("""
{
assert(global.size() == $size);
$test_entries
}
""")

    testLocal = Template("""
assert(locals.find("$name") != locals.end());
{
auto &local = locals["$name"];
assert(local.size() == $size);
$test_entries
}
""")

    def make_testEntry(self, e, table):
        assert isinstance(e, Entry)
        return self.testEntry.substitute(
            name=e.name,
            class_name=e.type.__class__.__name__,
            scope=pyenum2cppenum(e.scope),
            loc=", ".join(map(str, e.loc)),
            table=table,
        )

    def make_testLocal(self, name, local):
        return self.testLocal.substitute(
            name=name,
            size=len(local),
            test_entries="".join(self.make_testEntry(e, "local")
                                 for e in local.values()),
        )

    def make_testGlobal(self, global_):
        return self.testGlobal.substitute(
            size=len(global_),
            test_entries="".join(self.make_testEntry(e, "global")
                                 for e in global_.values()),
        )

    def substitute(self, symtable):
        return self.impl.substitute(
            test_global=self.make_testGlobal(symtable.global_),
            test_locals="".join(self.make_testLocal(name, local)
                                for name, local in symtable.locals_.items()),
        )


def symtable_make_test(symtable, file):
    """Generate a cpp test file using symtable"""
    code = format_code(CppTestTempalte().substitute(symtable))
    print(code, file=file)
