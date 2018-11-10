#! /usr/bin/env python3

"""Ast generation"""
import sys
import asdl
import util
from operator import attrgetter
from string import Template
from itertools import chain

class CppType:
    def __init__(self, type):
        self.type = type

    def __repr__(self):
        return "{0.__class__.__name__}({0.name!r})".format(self)

class AstNode(CppType):
    @property
    def arg(self):
        # pointer form
        return self.name + '*'

    member = arg

    @property
    def name(self):
        return self.type.capitalize()

    @property
    def seq_arg(self):
        return 'const std::vector<{}> &'.format(self.arg)

    @property
    def seq_member(self):
        return 'std::vector<{}>'.format(self.arg)


class Primitive(CppType):
    asdl2cpp = {
        'identifier': 'String',
        'string': 'String',
        'location': 'Location',
        'int': 'int',
    }

    @property
    def name(self):
        return self.asdl2cpp[self.type]

    @property
    def arg(self):
        if self.name == 'int':
            return self.name
        return 'const {} &'.format(self.name)

    @property
    def member(self):
        return self.name


class EnumClass(CppType):

    @property
    def name(self):
        return self.type.capitalize() + 'Kind'

    arg = member = name


class TypeVisitor(asdl.VisitorBase):
    """A visitor that map types from asdl to cpp."""

    def __init__(self):
        super().__init__()
        # hard coded known types
        self.typemap = {
            'identifier': Primitive('identifier'),
            'string': Primitive('string'),
            'location': Primitive('location'),
            'int': Primitive('int'),
        }

    def visitModule(self, mod):
        for dfn in mod.dfns:
            self.visit(dfn)

    def visitType(self, type):
        self.visit(type.value, str(type.name))

    def visitSum(self, sum, name):
        n_branches = len(sum.types)
        if n_branches > 1:
            self.typemap[name] = EnumClass(name)

        for t in sum.types:
            self.visit(t)

    def visitConstructor(self, cons):
        if cons.fields:
            self.typemap[cons.name] = AstNode(cons.name)

    def visitProduct(self, prod, name):
        self.typemap[name] = AstNode(name)


def get_typemap(mod):
    v = TypeVisitor()
    v.visit(mod)
    return v.typemap

def is_simple(sum):
    """Return True if a sum is a simple.

    A sum is simple if its types have no fields, e.g.
    unaryop = Invert | Not | UAdd | USub
    """
    return not any(t.fields for t in sum.types)


class AstEmittor(util.Emittor, asdl.VisitorBase):
    """Base class for Both an Emittor and a Visitor"""

    def __init__(self, file, typemap):
        super().__init__(file)
        self.typemap = typemap

    def visitModule(self, mod):
        for dfn in mod.dfns:
            self.visit(dfn)

    def get_cpp_type(self, name):
        return self.typemap[name]

    def get_args(self, fields, attributes):
        """Return (names, args, members) from ``fields`` and ``attributes``

        names is a list of names without type.
        args is a list of typed_name used as function arguments.
        members is a list of type_name used in member declaration.
        """

        names = []
        args = []
        members = []
        unnamed = {}

        for f in chain(fields, attributes):
            # prepare various types
            cpp_type = self.get_cpp_type(f.type)
            arg = cpp_type.seq_arg if f.is_seq else cpp_type.arg
            member = cpp_type.seq_member if f.is_seq else cpp_type.member

            # prepare the name
            if f.name is None:
                # field name is optional
                name = f.type
                c = unnamed[name] = unnamed.get(name, 0) + 1
                if c > 1:
                    name = "name{}".format(c - 1)
            else:
                name = f.name

            names.append(name)
            args.append("{type} {name}".format(arg, name))
            members.append("{type} {name}".format(member, name))

        return names, args, members


class ForwardEmittor(AstEmittor):
    """Forward declaration for all class (Constructor and Product)"""

    def visitType(self, type):
        # Product
        if isinstance(type.value, asdl.Product):
            self.emit("class {name};".format(
                name=self.get_cpp_type(type.name).name))
            self.emit("")
        # Sum
        if isinstance(type.value, asdl.Sum):
            self.visit(type.value)

    def visitSum(self, sum):
        for cons in sum.types:
            if not cons.fields:
                continue
            self.emit("class {name};".format(
                name=self.get_cpp_type(cons.name).name))
            self.emit("")


class EnumEmittor(AstEmittor):
    """Emit ``enum class`` for sums"""

    def visitType(self, node):
        if isinstance(node.value, asdl.Sum):
            self.visit(node.value, node.name)

    def visitSum(self, sum, name):
        if len(sum.types) == 1:
            return
        # field_names are all int constant, no need get_args()
        field_names = [c.name for c in sum.types]
        enum_name = self.get_cpp_type(name).name

        self.emit("enum class {name} {{ {constants} }};".format(
            name=enum_name,
            constants=", ".join(field_names)))
        self.emit("")

        self.emit(
        "inline std::ostream &operator<<(std::ostream &os, {name} val) {{".format(
            name=enum_name))
        self.emit("switch (val) {", 1)
        for n in field_names:
            self.emit("case {enum}::{name}: os << \"{name}\";".format(enum=enum_name,
                name=n), 1)
        self.emit("}", 1)
        self.emit("return os;", 1)
        self.emit("}\n")


class ClassEmittor(AstEmittor):
    """Emit class for Constructor"""

    template = Template("""
class $name: public AST {
public:
    $members

    $name($parameters): $init {}

    int SubclassKind() const override {
        return $enum::$name;
    }

    void Format(std::ostream &os) const override {
        os << "$name(";
        $format_fields
    }
};
""")

    def visitType(self, type):
        if not isinstance(type, asdl.Sum):
            return
        for cons in type.value.types:
            self.visit(cons, type.name, type.value.attributes)

    def visitConstructor(self, cons, sum_name, attributes):
        if not cons.fields:
            return
        class_name = self.get_cpp_type(cons.name).name
        typed_args, args = self.get_args(cons.fields, attributes);
        members = [arg + ";" for arg in typed_args]
        parameters = ", ".join(typed_args)
        init = ", ".join("{0}({0})".format(arg) for arg in args)
        format_fields = ["os << \"{0}=\" << {0};".format(arg) in args]

        self.emit_raw(self.template.substitute(
            name=class_name,
            enum=self.get_cpp_type(sum_name).name,
            members=members,
            parameters=parameters,
            init=init,
            format_fields=util.indented_lines(format_fields, 2),
        ))


class StructEmittor(AstEmittor):
    """Emit ``struct`` for Product"""

    template = Template("""
struct $name {
    $members

    $name($parameters): $init {}
};

inline std::ostream &operator<<(std::ostream &os, const $name *ast) {
    os << "$name(";
    $format_fields
    return os;
}
""")

    def visitType(self, type):
        if not isinstance(type, asdl.Product):
            return
        self.visit(type.value, type.name, type.value.attributes)

    def visitProduct(self, prod, sum_name, attributes):
        names, args, members = self.get_args(prod.fields, attributes)
        name = self.get_cpp_type(sum_name).name
        init = ", ".join("{0}({0})".format(arg) for arg in args)
        format_fields = ["os << \"{0}=\" << {0};".format(arg) in args]

        self.emit_raw(self.template.substitute(
            name=name,
            members=util.indented_lines(members),
            parameters=args,
            init=init,
            format_fields=format_fields,
        ))


Header = """#include "tokenize.h"

#include <vector>
#include <iostream>

class AST {
public:
    virtual int SubclassKind() const = 0;
    virtual void Format(std::ostream &os) const = 0;
};

inline std::ostream &operator<<(std::ostream &os, const AST *ast) {
    ast->Format(os);
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const AST &ast) {
    return os << &ast;
}

"""

class String2enumEmittor(AstEmittor):

    table = {
    'operator': {
        "+": "Add",
        "-": "Sub",
        "*": "Mult",
        "/": "Div",
        "==": "Eq",
        "!=": "NotEq",
        "<": "Lt",
        "<=": "LtE",
        ">": "Gt",
        ">=": "GtE",
    },

    'unaryop': {
        "+": "UAdd",
        "-": "USub",
    },

    'basic_type': {
        "int": "Int",
        "char": "Character",
        "void": "Void",
    },
    }

    def emit_(self):
        for enum, data in self.table.items():
            enum_name = self.get_cpp_type(enum).name
            self.emit("inline {enum} {funcname}(const String &s) {{".format(
                enum=enum_name,
                funcname='String2' + enum_name,
            ))
            for op, val in data.items():
                self.emit("if (s == \"{op}\") return {enum}::{val};".format(
                    op=op, enum=enum_name, val=val), 1)
                self.emit("")
            self.emit("assert(false && \"not a member of {enum}\");".format(enum_name), 1)
            self.emit("}")
            self.emit("")


def main():
    import argparse
    import json
    from pprint import pprint

    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--config', dest='config', type=argparse.FileType(),
            help='configure file', required=1)
    args = parser.parse_args()
    config = json.load(args.config)

    mod = asdl.parse(config['asdl'])
    if not asdl.check(mod):
        return 1

    typemap = get_typemap(mod)
    # pprint(typemap)
    # return
    # with open(config['AST.h'], 'w') as f:
    with sys.stdout as f:
        f.write(Header)
        c = util.ChainOfVisitors(
            # ForwardEmittor(f, typemap),
            EnumEmittor(f, typemap),
            # ClassEmittor(f, typemap),
            # StructEmittor(f, typemap),
            # String2enumEmittor(f, typemap)
        )
        c.visit(mod)
    return 0


if __name__ == '__main__':
    sys.exit(main())
