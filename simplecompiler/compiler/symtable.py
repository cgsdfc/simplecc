"""Symbol table construction for the program"""

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
from simplecompiler.util import error
Arg = arg

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


def make_local(fun, top):
    dict = {}
    ok = 1
    # insert all arguments first
    for arg in fun.args:
        if arg.name in dict:
            ok = 0
            error("redefinition of argument {!r} in function {!r}".format(
                arg.name, fun.name), arg.loc)
        else:
            dict[arg.name] = Entry(
                Variable(arg.type), Scope.Local, arg.loc, arg.name)

    # then local const/var decl
    for decl in fun.decls:
        if decl.name in dict:
            ok = 0
            error("redefinition of identifier {!r} in function {!r}".format(
                decl.name, fun.name), decl.loc)
        else:
            dict[decl.name] = Entry(decl_to_type(
                decl), Scope.Local, decl.loc, decl.name)

    # check uses
    for name, loc in iter_names(fun):
        if name in dict:
            continue
        if name in top and top[name].loc <= loc:
            dict[name] = Entry(top[name].type, Scope.Global, loc, name)
        else:
            ok = 0
            error("undefined identifier {!r} in function {!r}".format(
                name, fun.name), loc)

    return dict if ok else None


def make_global(prog):
    dict = {}
    ok = 1
    for decl in prog.decls:
        if decl.name in dict:
            ok = 0
            error("redefinition of identifier {!r} in {}".format(
                decl.name, MODULE_NAME), decl.loc)
        else:
            dict[decl.name] = Entry(decl_to_type(
                decl), Scope.Global, decl.loc, decl.name)
    return dict if ok else None

# now we have the type information collected.
# we still need scope information.
# the algorithm is simple --
# first all names in global namespace is of global scope themselves.
# second, in a function's local namespace, a name's scope
#   1. is local if it is in the local typemap;
#   2. or else, is global if it is in the global typemap;
#   3. or else, is an error (undefined)
# finally, type and scope information are put in the Entry namedtuple


# An Entry represents a single resolved symbol
Entry = namedtuple("Entry", "type scope loc name")


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

    def lookup(self, name, ns_name=None):
        """Lookup a symbol (its Entry) using name. If ns_name is given, perform the
        lookup under the local namespace. Or else the global namespace
        is searched.
        """
        if ns_name is None:
            return self.global_[name]
        else:
            return self.locals_[ns_name][name]

    def get_identifiers(self, ns_name=None):
        return self.global_.keys() if ns_name is None else \
            self.locals_[ns_name].keys()

    def report(self):
        print("global:")
        pprint(sorted(self.global_.values(), key=attrgetter('loc')))
        print("locals:")
        pprint(self.locals_)


def make_locals(funcdefs, top):
    dict = {}
    ok = 1
    for fun in funcdefs:
        local = make_local(fun, top)
        if local is None:
            ok = 0
        else:
            dict[fun.name] = local
    return dict if ok else None


def build_symtable(prog):
    """Build a SymbolTable for the whole program"""
    assert isinstance(prog, Program)

    top = make_global(prog)
    if top is None:
        return None
    funcdefs = filter(lambda d: isinstance(d, FuncDef), prog.decls)
    locals_ = make_locals(funcdefs, top)
    if locals_ is None:
        return None
    return SymbolTable(top, locals_)


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
const Entry &e = $table["$name"];
assert(IsInstance<$class_name>(e.type));
assert(e.scope == $scope);
assert(e.name == "$name");
// assert(e.location == Location($loc));
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
