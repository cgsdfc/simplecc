from functools import singledispatch
from enum import Enum

from AST import *

Scope = Enum("Scope", "Local Global")

# pure data class to hold type information
class Constant:

    def __init__(self, type):
        self.type = type

class Variable:

    def __init__(self, type):
        self.type = type

class Array:

    def __init__(self, elemtype, size):
        self.elemtype = elemtype
        self.size = size

class Function:

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
    type = decl.var_type
    return Array(type.type, type.size) if type.is_array else Variable(type.type)

@decl_to_type(FuncDef)
def FuncDef2Type(decl):
    args = [decl_to_type(type) for type, _ in decl.args]
    return Function(decl.return_type, args)


def make_typemap_prog(prog):
    """Return a pair -- first is the global typemap and its name, the second is
    a dict mapping function names to their local typemap.
    """

    def make_typemap(decls):
        """Convert a series of declarations to a dict mapping names to
        their type.
        """
        return {decl.name: decl_to_type(decl) for decl in decls}

    top = ("<module>", make_typemap(prog.decls))
    def make_locals():
        funcdefs = filter(lambda d: isinstance(d, FuncDef), prog.decls)
        for fun in funcdefs:
            yield (fun.name, make_typemap(fun.decls))
    return top, dict(make_locals())

# now we have the type information collected
# we still need scope information
# the algorithm is simple --
# first all names in global namespace is global them themselves
# second, in a function's local namespace, a name
#   1. is local if it is in the local typemap;
#   2. or else, is global if it is in the global typemap;
#   3. or else, is an error (undefined)
# finally, type and scope information are put in the Entry namedtuple

Entry = namedtuple("Entry", "type scope loc")

def extract_all_names(node):
    """Extract all (name, loc) pair in node"""

    def iter_names(node):
        if isinstance(node, (decl, arg)):
            yield node.name, node.loc
        elif isinstance(node, (Read,)):
            for name in node:
                yield name, node.loc
        elif isinstance(node, Call):
            yield node.func, node.loc
        elif isinstance(node, Name):
            yield node.id, node.loc
        elif isinstance(node, Subscript):
            yield node.name, node.loc
        else:
            # non-terminal node
            # work for any iterable of nodes
            for child in node:
                yield from iter_names(child)

    return list(iter_names(node))


def resolve_local_names(ns_name, local, global_, names):
    """Resolve local names using both local and global typemaps.
    Return the complete symtable if success, else return None.
    """
    out = {}
    ok = 1
    for name, loc in names:
        if name in local:
            out[name] = Entry(local[name], Scope.Local, loc)
        elif name in global_:
            out[name] = Entry(local[name], Scope.Global, loc)
        else:
            ok = 0
            error("undefined identifier {!r} in function {!r}".format(
                name, ns_name), loc)
    return out if ok else None


def build_local_tables(prog, top, locals_):
    funcdefs = filter(lambda n: isinstance(n, FuncDef), prog.decls)
    resolved = {}
    ok = 1

    for fun in funcdefs:
        names = extract_all_names(fun)
        local = resolve_local_names(fun.name, locals_[name], top, names)
        if local is None:
            ok = 0
        else:
            resolved[fun.name] = local

    return resolved if ok else None


def build_global_table(prog, typemap):
    """Add scope and location information to the global typemap"""
    def iter_name_loc():
        for child in prog.decls:
            yield child.name, child.loc

    return {name: Entry(typemap[name], Scope.Global, loc)
            for name, loc in iter_name_loc()}


class SymbolTableEntry:
    """Simple container about type, scope information of each name
    in a block -- Module and Function block
    """

    def __init__(self, name, table):
        self.name = name
        self.table = table

    def __getitem__(self, name):
        return self.table[name]


class SymbolTable:
    """All-in-one container of all names and their information within a program.
    """

    def __init__(self, global_, locals_):
        self.global_ = global_
        self.locals_ = locals_

    def lookup(self, name, ns_name=None):
        """Lookup a symbol using name. If ns_name is given, perform the
        lookup under the local namespace. Or else the global namespace
        is searched.
        """
        if ns_name is None:
            return self.global_[name]
        else:
            return self.locals_[ns_name][name]


def build_symtable(prog):
    # build typemap for both global and local
    top, locals_ = make_typemap_prog(prog)

    # resolve local names first
    locals_ = build_local_tables(prog, top, locals_)
    if locals_ is None:
        return None
    # turn into SymbolTableEntry dict
    locals_ = {name: SymbolTableEntry(name, table)
            for name, table in locals_.items()}

    # build global table
    global_ = build_global_table(prog, top)
    global_ = SymbolTableEntry(top[0], global_)

    # all set
    return SymbolTable(global_, locals_)

