#! /usr/bin/env python3

"""Ast generation"""

import sys
import os
import subprocess
import tempfile
from string import Template
from pprint import pprint
from itertools import chain

import asdl

def camal_case(name):
    """Convert snake_case to camal_case, respecting the original
    camals in ``name``
    """
    # uppercase the first letter, keep the rest unchanged
    return ''.join(c[0].upper() + c[1:] for c in name.split('_'))


def is_simple(sum):
    """Return True if a sum is a simple.

    A sum is simple if its types have no fields, e.g.
    unaryop = Invert | Not | UAdd | USub
    """
    return not any(t.fields for t in sum.types)


class CppType:
    """Base type for a cpp type that goes into output"""

    def const_ref(self, name):
        return 'const {}&'.format(name)

    def ptr(self, name):
        return name + '*'

    def __repr__(self):
        # taking subclass's name
        return "{}({!r})".format(self.__class__.__name__, self.name)

    # Our name is unique across cpp project, so as asdl file
    def __eq__(self, other):
        return isinstance(other, self.__class__) and self.name == other.name

    def __hash__(self):
        return hash(self.name)

    def delete(self, name):
        """delete stmt in cpp"""
        return None


class AstNode(CppType):
    """AstNode represents subclasses of AST"""

    delete_ = Template("""delete $name;""")

    def __init__(self, name):
        self.name = camal_case(name)

    @property
    def as_member(self):
        return self.ptr(self.name)

    as_argument = as_member

    def delete(self, name):
        return self.delete_.substitute(name=name)

class Primitive(CppType):
    """Primitive types in both asdl and cpp"""

    strimpl = 'std::string'
    asdl2cpp = {
        'identifier': strimpl,
        'string': strimpl,
        'location': 'Location',
        'int': 'int',
    }

    def __init__(self, name, is_basic=False):
        self.name = self.asdl2cpp[name]
        self.is_basic = is_basic

    @property
    def as_member(self):
        return self.name

    @property
    def as_argument(self):
        if self.is_basic:
            # too verbose to say "const int&"
            return self.as_member
        else:
            # larger types
            return self.const_ref(self.name)


# These are simple data container about cpp types
class AbstractNode(AstNode):
    """Direct subclasses of AST, virtual base class for ConcreteNode"""

    extra_members = [ (Primitive('int', True), 'subclass_tag'), ]

    def __init__(self, name):
        super().__init__(name)
        self.subclasses = None
        self.members = None

    @property
    def members(self):
        """All members, including subclass_tag"""
        return self.extra_members + self._members

    @property
    def members_notag(self):
        """Members excluding subclass_tag"""
        return self._members

    @members.setter
    def members(self, val):
        self._members = val


class ConcreteNode(AstNode):
    """Direct subclass of AbstractNode, indirectly subclassing AST"""

    def __init__(self, name, base=None, members=None):
        super().__init__(name)
        self.base = base
        self.members = members


class LeafNode(AstNode):
    """Direct subclass of AST, non-virtual class"""

    def __init__(self, name, members=None):
        super().__init__(name)
        self.members = members


class EnumClass(CppType):
    """Plain enum class"""

    def __init__(self, name, values=None, strings=None):
        self.name = camal_case(name) + 'Kind'
        self.values = values
        self.strings = strings

    @property
    def as_member(self):
        # enums are just ints
        return self.name

    as_argument = as_member


class Sequence(CppType):
    """A list of item dealing with * in asdl"""

    impl = 'std::vector<{}>'

    delete_seq = Template("""for (auto v: $name) delete v;""")

    def __init__(self, elemtype):
        self.elemtype = elemtype

    @property
    def as_member(self):
        return self.impl.format(self.elemtype.as_member)

    @property
    def as_argument(self):
        return self.const_ref(self.as_member)

    def delete(self, name):
        if isinstance(self.elemtype, AstNode):
            return self.delete_seq.substitute(name=name)
        return None

    name = as_member


class Optional(CppType):
    """A optional item dealing with ? in asdl"""

    impl = 'std::optional<{}>'
    delete_opt = Template("""if ($name) delete $name;""")

    def __init__(self, elemtype):
        self.elemtype = elemtype

    @property
    def as_member(self):
        if isinstance(self.elemtype, AstNode):
            # nullptr serves a great optional
            return self.elemtype.as_member
        else:
            return self.impl.format(self.elemtype.as_member)

    as_argument = as_member
    name = as_member

    def delete(self, name):
        if isinstance(self.elemtype, AstNode):
            return self.delete_opt.substitute(name=name)
        return None


class TypeVisitor(asdl.VisitorBase):
    """A visitor that map types from asdl to cpp."""

    def __init__(self):
        super().__init__()
        # hard coded known types
        self.typemap = {
            'identifier': Primitive('identifier', False),
            'string': Primitive('string', False),
            'location': Primitive('location', True),
            'int': Primitive('int', True),
        }

    def visitModule(self, mod):
        for dfn in mod.dfns:
            self.visit(dfn)

    def visitType(self, type):
        self.visit(type.value, type.name)

    def visitSum(self, sum, name):
        # EnumClass
        if is_simple(sum):
            self.typemap[name] = EnumClass(name)
            return

        # LeafNode
        if len(sum.types) == 1:
            cons = sum.types[0]
            self.typemap[cons.name] = self.typemap[name] = LeafNode(name)
            return

        # AbstractNode
        self.typemap[name] = AbstractNode(name)

        # ConcreteNode
        for cons in sum.types:
            assert cons.fields, "Mixture of simple and complex sum not allowed"
            self.typemap[cons.name] = ConcreteNode(cons.name)


    def visitProduct(self, prod, name):
        # LeafNode
        self.typemap[name] = LeafNode(name)


class String2Enum:
    """Namespace for string2enum hard-coded constants"""
    # make sure these names match those in asdl!
    operator = {
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
    }

    unaryop = {
        "+": "UAdd",
        "-": "USub",
    }

    basic_type = {
        "int": "Int",
        "char": "Character",
        "void": "Void",
    }


class TypeVisitor2(asdl.VisitorBase):
    """The second pass TypeVisitor, resolve members and base"""

    def __init__(self, typemap):
        super().__init__()
        self.typemap = typemap
        # names to remove from typemap after visiting
        self.to_remove = []

    def remove(self):
        # call this after visit()
        for name in self.to_remove:
            del self.typemap[name]

    def visitModule(self, mod):
        for dfn in mod.dfns:
            self.visit(dfn)

    def visitType(self, type):
        self.visit(type.value, type.name)

    def visitSum(self, sum, name):
        # EnumClass
        if is_simple(sum):
            type = self.typemap[name]
            assert isinstance(type, EnumClass)
            type.values = [cons.name for cons in sum.types]
            type.strings = getattr(String2Enum, name, None)
            return

        # LeafNode
        if len(sum.types) == 1: # direct subclass of AST -- LeafNode
            self.to_remove.append(name) # its sum name won't be generated
            cons = sum.types[0]
            type = self.typemap[cons.name]
            assert isinstance(type, LeafNode)
            type.members = [(self.field_type(f), f.name) for f in chain(cons.fields, sum.attributes)]
            return

        # AbstractNode
        type = self.typemap[name]
        assert isinstance(type, AbstractNode)
        type.subclasses = [self.typemap[cons.name] for cons in sum.types]
        type.members = [(self.field_type(f), f.name) for f in sum.attributes]

        # ConcreteNode
        for cons in sum.types:
            self.visit(cons, type)


    def visitConstructor(self, cons, base):
        assert cons.fields, "Mixture of simple and complex sum not allowed"
        node = self.typemap[cons.name]
        assert isinstance(node, ConcreteNode)
        node.base = base
        node.members = [(self.field_type(f), f.name) for f in cons.fields]

    def field_type(self, f):
        type = self.typemap[f.type]
        if f.seq: return Sequence(type)
        if f.opt: return Optional(type)
        return type

    def visitProduct(self, prod, name):
        # LeafNode
        type = self.typemap[name]
        assert isinstance(type, LeafNode)
        type.members = [(self.field_type(f), f.name)
                for f in chain(prod.fields, prod.attributes)]


def make_typemap(mod):
    v1 = TypeVisitor()
    v1.visit(mod)
    v2 = TypeVisitor2(v1.typemap)
    v2.visit(mod)
    v2.remove()
    return v2.typemap


# Templates for code blocks
class HeaderTemplate:
    decl = Template("""
#ifndef AST_H
#define AST_H

#include "tokenize.h"

#include <vector>
#include <iostream>
#include <optional>

class AST {
public:
    virtual void Format(std::ostream &os) const = 0;
    virtual ~AST() = 0;
    virtual String ClassName() const = 0;
};

inline AST::~AST() {}

inline std::ostream &operator<<(std::ostream &os, const AST *ast) {
    if (ast == nullptr)
        return os << "None";
    ast->Format(os);
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const AST &ast) {
    return os << &ast;
}

// ForwardDecl
$forward_declarations

// EnumClass
$enum_classes

// AbstractNode
$abstract_classes

// ConcreteNode
$concrete_classes

// LeafNode
$leafnode_classes

// String2Enum
$string2enum_decls

template<typename T, typename U>
inline bool IsInstance(U *x) {
    return T::InstanceCheck(x);
}

template<typename T, typename U>
inline T *subclass_cast(U *x) {
    if (IsInstance<T>(x))
        return static_cast<T*>(x);
    return nullptr;
}

$visitor_base
#endif
""")

    forward_decl = Template("""class $class_name;""")

    @classmethod
    def make_forward_decls(cls, typemap):
        """Return an iterable of forward_decl strings"""
        for x in typemap.values():
            if isinstance(x, AstNode):
                yield cls.forward_decl.substitute(class_name=x.name)


    @classmethod
    def substitute(cls, typemap):
        return cls.decl.substitute(
            forward_declarations="\n".join(cls.make_forward_decls(typemap)),
            enum_classes="\n".join(substitute_all(typemap, EnumClass)),
            abstract_classes="\n".join(substitute_all(typemap, AbstractNode)),
            concrete_classes="\n".join(substitute_all(typemap, ConcreteNode)),
            leafnode_classes="\n".join(substitute_all(typemap, LeafNode)),
            string2enum_decls="\n".join(String2EnumTempalte.substitute_decls(typemap)),
            visitor_base=VisitorBaseTemplate.substitute(typemap),
        )

def substitute_all(typemap, match, method="substitute"):
    """Substitute all types matching ``match`` and iterate over them"""
    return map(lambda x: substitute(x, method),
        filter(lambda x: isinstance(x, match), typemap.values()))

def substitute(x, method="substitute"):
    """Turn a cpp type ``x`` into substituted string"""
    template = eval(x.__class__.__name__ + "Template")
    return getattr(template, method)(x)


class ImplTemplate:
    impl = Template("""
#include "AST.h"

template<class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
    os << "[";
    for (auto b = v.begin(), e = v.end(); b != e; ++b) {
        os << *b;
        if (b != e - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

template<class T>
std::ostream &operator<<(std::ostream &os, const std::optional<T> &v) {
    if (v.has_value())
        return os << v.value();
    else
        return os << "None";
}

$formatter_impls
$destructor_impls
$string2enum_impls
""")

    @classmethod
    def substitute(cls, typemap):
        return cls.impl.substitute(
            formatter_impls="\n".join(substitute_all(
                typemap,
                (ConcreteNode, LeafNode),
                'make_formatter',
            )),
            destructor_impls="\n".join(substitute_all(
                typemap,
                (ConcreteNode, LeafNode),
                'make_destructor',
            )),
            string2enum_impls="\n".join(
                String2EnumTempalte.substitute_impls(typemap)
            ),
        )



class AbstractNodeTemplate:
    decl = Template("""
class $class_name: public AST {
public:
    $member_declaration

    $class_name($constructor_args): AST(), $member_init {}

    enum {$enumitems};
};
""")
    @classmethod
    def substitute(cls, x):
        return cls.decl.substitute(
            class_name=x.name,
            member_declaration=make_member_decls(x.members),
            constructor_args=make_formal_args(x.members),
            member_init=make_init(x.members),
            enumitems=cls.make_enumitems(x.subclasses),
        )

    @classmethod
    def make_enumitems(cls, subclasses):
        return ", ".join(c.name for c in subclasses)


class ClassImplTemplate:
    formatter_impl = Template("""
void $class_name::Format(std::ostream &os) const {
    os << "$class_name(" <<
    $code
    << ")";
}
""")

    destructor_impl = Template("""
$class_name::~$class_name() {
    $code
}
""")

    format_item = Template(""" "$field_name=" << $expr """)

    os_joiner = """ << ", " << """

    @classmethod
    def make_formatter(cls, x):
        return cls.formatter_impl.substitute(
            class_name=x.name,
            code=cls.os_joiner.join(
                cls.format_item.substitute(
                    field_name=name,
                    # expr='*' + name if isinstance(type, AstNode) else name
                    expr=name,
                ) for type, name in x.members
            )
        )

    @classmethod
    def make_destructor(cls, x):
        delete_stmts = list(
                filter(None, [type.delete(name) for type, name in x.members]))
        return cls.destructor_impl.substitute(
            class_name=x.name,
            code="\n".join(delete_stmts)
        )

    @classmethod
    def substitute_impl(cls, x):
        yield cls.make_formatter(x)
        yield cls.make_destructor(x)


class ConcreteNodeTemplate(ClassImplTemplate):
    decl = Template("""
class $class_name: public $base {
public:
    $member_declaration

    $class_name($constructor_args):
        $base($base::$class_name, $base_init), $member_init {}

    ~$class_name() override;

    String ClassName() const override {
        return "$class_name";
    }

    void Format(std::ostream &os) const override;

    static bool InstanceCheck($base *x) {
        return x->subclass_tag == $base::$class_name;
    }
};
""")

    @classmethod
    def substitute(cls, x):
        assert isinstance(x, ConcreteNode)
        return cls.decl.substitute(
            class_name=x.name,
            base=x.base.name,
            member_declaration=make_member_decls(x.members),
            constructor_args=make_formal_args(x.members + x.base.members_notag),
            base_init=make_actual_args(x.base.members_notag),
            member_init=make_init(x.members),
        )


class LeafNodeTemplate(ClassImplTemplate):
    decl = Template("""
class $class_name: public AST {
public:
    $member_declaration

    $class_name($constructor_args): AST(), $member_init {}

    ~$class_name() override;

    String ClassName() const override {
        return "$class_name";
    }

    void Format(std::ostream &os) const override;
};
""")

    @classmethod
    def substitute(cls, x):
        assert isinstance(x, LeafNode)
        return cls.decl.substitute(
            class_name=x.name,
            member_declaration=make_member_decls(x.members),
            constructor_args=make_formal_args(x.members),
            member_init=make_init(x.members),
        )


class EnumClassTemplate:
    decl = Template("""
enum class $class_name { $enumitems };

inline std::ostream &operator<<(std::ostream &os, $class_name val) {
    switch (val) {
    $case_stmts
    }
}
""")

    case_stmt = Template("""
case $class_name::$item: return os << "$class_name::$item";
""")

    @classmethod
    def substitute(cls, x):
        assert isinstance(x, EnumClass)
        return cls.decl.substitute(
            class_name=x.name,
            enumitems=make_enumitems(x.values),
            case_stmts=cls.make_case_stmts(x),
        )

    @classmethod
    def make_case_stmts(cls, x):
        return "\n".join(cls.case_stmt.substitute(
            class_name=x.name,
            item=item) for item in x.values)



class String2EnumTempalte:
    decl = Template("""
$class_name String2$class_name(const String &s);
""")

    impl = Template("""
$class_name String2$class_name(const String &s) {
    $conditions
    assert(false && "not a member of $class_name");
}
""")

    condition = Template("""
if (s == "$string")
    return $class_name::$item;
""")

    @staticmethod
    def make_string2enums(typemap):
        """Filter the enums that needs string2enum()"""
        return filter(lambda x: isinstance(x, EnumClass) and x.strings,
                typemap.values())

    @classmethod
    def substitute_decls(cls, typemap):
        for x in cls.make_string2enums(typemap):
            yield cls.decl.substitute(class_name=x.name)

    @classmethod
    def substitute_impls(cls, typemap):
        for x in cls.make_string2enums(typemap):
            yield cls.impl.substitute(
                    class_name=x.name,
                    conditions="\n".join(
                        cls.condition.substitute(
                            class_name=x.name,
                            item=item,
                            string=string,
                        )
                    for string, item in x.strings.items()
                )
            )



class VisitorBaseTemplate:
    decl = Template("""
// CRTP-based visitor base implementing dispatch on node type.
template <typename Derived>
class VisitorBase {
public:
$abstract_visitor
};
""")

    abstract = Template("""
template<typename R, typename... Args>
R visit($class_name *node, Args&&... args) {
    $test_and_dispatches
    assert(false && "$class_name");
}
""")

    dispatch = Template("""
if (auto x = subclass_cast<$class_name>(node))
    return static_cast<Derived*>(this)->visit(node, args...);
""")

    @classmethod
    def make_abstract_visitor(cls, x):
        assert isinstance(x, AbstractNode)
        return cls.abstract.substitute(
            class_name=x.name,
            test_and_dispatches="\n".join(
                cls.dispatch.substitute(class_name=sub.name) for sub in x.subclasses
            )
        )

    @classmethod
    def substitute(cls, typemap):
        return cls.decl.substitute(
            abstract_visitor="\n".join(cls.make_abstract_visitor(x)
                for x in typemap.values() if isinstance(x, AbstractNode))
        )


# helpers
def make_formal_args(members):
    """Comma-separated type name pair used in formal arguments

    int i, char j
    """
    return ", ".join("{} {}".format(type.as_argument, name)
            for type, name in members)

def make_init(members):
    """Comma-separated name(name) list used in initializer list
    in constructor.

    value(value), type(type)
    """
    return ", ".join("{0}({0})".format(name) for _, name in members)

def make_actual_args(members):
    """Comma-separated name list used in actual arguments

    a, b, c
    """
    return ", ".join(name for _, name in members)

def make_member_decls(members):
    """Semicolon-terminated type name list used in member declaration.

    int i;
    char j;
    """
    return "\n".join("{} {};".format(
        type.as_member,
        name
    ) for type, name in members)

def make_enumitems(values):
    """Comma-separated names used in enum items.

    APPLE, BANANA
    """
    return ", ".join(values)


def format_code(code_string, dest, external_formatter=None):
    """Format C++ code in ``code_string`` using ``external_formatter`` and
    write it to ``dest``.
    external_formatter is the program to use, default to clang-format.
    """
    if external_formatter is None:
        external_formatter = "clang-format"

    fd, temp = tempfile.mkstemp()
    with open(temp, 'w') as f:
        f.write(code_string)

    try:
        formatted = subprocess.check_output([external_formatter, temp])
        with open(dest, 'wb') as f:
            f.write(formatted)
    finally:
        os.remove(temp)


def generate_code(typemap, template_class, dest):
    format_code(template_class.substitute(typemap), dest)


def main():
    import argparse
    import json

    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--config', dest='config', type=argparse.FileType(),
            help='configure file', required=1)
    parser.add_argument('-d', '--dump-typemap', dest='dump_typemap', action='store_true',
            default=False)
    args = parser.parse_args()
    config = json.load(args.config)

    mod = asdl.parse(config['AST']['asdl'])
    if not asdl.check(mod):
        return 1

    typemap = make_typemap(mod)
    if args.dump_typemap:
        pprint(typemap)
        return 0

    generate_code(typemap, HeaderTemplate, config['AST']['AST.h'])
    generate_code(typemap, ImplTemplate, config['AST']['AST.cpp'])

    return 0


if __name__ == '__main__':
    sys.exit(main())
