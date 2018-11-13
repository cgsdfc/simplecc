#! /usr/bin/env python3

"""Ast generation"""

import sys
import asdl
import util
import abc

from string import Template
from pprint import pprint
from itertools import chain


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
        return self.delete_.substitute(name)

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

    methods = ('constructor', 'destructor')
    # extra_members = [(Primitive('int'), 'subclass_tag')]
    class_or_struct = 'class'

    def __init__(self, name, members=None, subclasses=None):
        super().__init__(name)
        self.subclasses = subclasses
        self.members = members

    # def __repr__(self):
    #     return "AbstractNode(name={}, members={}, subclasses={})".format(
    #         self.name,
    #         self.members,
    #         self.subclasses,
    #     )



class ConcreteNode(AstNode):

    methods = ('constructor', 'destructor', 'formatter')
    class_or_struct = 'class'

    def __init__(self, name, base=None, members=None):
        super().__init__(name)
        self.base = base
        self.members = members

    # def __repr__(self):
    #     return "ConcreteNode(name={}, base={}, members={})".format(
    #         self.name,
    #         self.base,
    #         self.members,
    #     )


class ProductStruct(AstNode):

    methods = ('constructor', 'destructor', 'formatter')
    class_or_struct = 'struct'

    def __init__(self, name, members=None):
        super().__init__(name)
        self.members = members

    # def __repr__(self):
    #     return "ProductStruct(name={}, members={})".format(
    #         self.name,
    #         self.members,
    #     )


class EnumClass(CppType):

    methods = ('formatter', 'string2enum')

    def __init__(self, name, values=None, strings=None):
        self.name = camal_case(name) + 'Kind'
        self.values = values
        self.strings = strings

    @property
    def as_member(self):
        # enums are just ints
        return self.name

    as_argument = as_member

    # def __repr__(self):
    #     return "EnumClass(name={}, values={})".format(
    #         self.name,
    #         " ".join(self.values),
    #     )



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
            return self.delete_seq.substitute(name)
        return None

    name = as_member


class Optional(CppType):
    """A optional item dealing with ? in asdl"""

    impl = 'std::optional<{}>'
    delete_opt = Template("""if ($name) delete $name""")

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
            return self.delete_opt.substitute(name)
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
        if is_simple(sum):
            self.typemap[name] = EnumClass(name)
            return

        if len(sum.types) == 1:
            self.typemap[name] = ConcreteNode(name)
        else:
            self.typemap[name] = AbstractNode(name)

        for cons in sum.types:
            assert cons.fields, "Mixture of simple and complex sum not allowed"
            self.typemap[cons.name] = ConcreteNode(cons.name)


    def visitProduct(self, prod, name):
        self.typemap[name] = ProductStruct(name)


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
    """The second pass TypeVisitor"""

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
        if is_simple(sum):
            type = self.typemap[name]
            assert isinstance(type, EnumClass)
            type.values = [cons.name for cons in sum.types]
            try:
                type.strings = getattr(String2Enum, name)
            except AttributeError:
                pass
            return

        if len(sum.types) == 1:
            # solo ConcreteNode, only used for resolution, not goes into cpp
            self.to_remove.append(name)
            return

        type = self.typemap[name]
        assert isinstance(type, AbstractNode)
        type.subclasses = [self.typemap[cons.name] for cons in sum.types]
        type.members = [(self.field_type(f), f.name) for f in sum.attributes]

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
        type = self.typemap[name]
        assert isinstance(type, ProductStruct)
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


$forward_declarations

$enum_classes

$abstract_classes

$concrete_classes

$string2enum_decls

template<typename T, typename U>
inline bool IsInstance(U *x) {
    return T::InstanceCheck(x);
}

template<typename T, typename U>
inline T *subclass_cast(U *x) {
    if (IsInstance<T>(x)) return static_cast<T*>(x);
    return nullptr;
}

#endif
""")

    forward_decl = Template("""{class_or_struct} {class_name};""")

    @classmethod
    def make_forward_decls(cls, typemap):
        """Return an iterable of forward_decl strings"""
        for x in typemap.values():
            try:
                # class_or_struct needs forward_decl
                yield cls.forward_decl.substitute(
                    class_or_struct=x.class_or_struct,
                    class_name=x.name
                )
            except AttributeError:
                pass


    @classmethod
    def substitute(cls, typemap):
        return cls.decl.substitute(
            forward_declarations="\n".join(cls.make_forward_decls(typemap)),
            enum_classes="\n".join(substitute_all(typemap, EnumClass)),
            abstract_classes="\n".join(substitute_all(typemap, AbstractNode)),
            concrete_classes="\n".join(substitute_all(typemap, ConcreteNode)),
            string2enum_decls="\n".join(String2EnumTempalte.substitute_decls(typemap)),
        )

def substitute_all(typemap, match, method="substitute"):
    """Substitute all types matching ``match`` and iterate over them"""
    return map(lambda x: substitute(x, method),
        filter(lambda x: isinstance(x, match), typemap.values()))

def substitute(x, method="substitute"):
    """Turn a cpp type ``x`` into substituted string"""
    template = eval(x.__class__.__name__ + "Template")
    return getattr(template, method)(x)


class CppTemplate:
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

$concrete_impls

$string2enum_impls
""")

    @classmethod
    def substitute(cls, typemap):
        return cls.impl.substitute(
            concrete_impls=substitute_all(
                typemap, ConcreteNodeTemplate, 'substitute_impl'),
            string2enum_impls=String2EnumTempalte.substitute_impls(typemap),
        )



class AbstractNodeTemplate:
    decl = Template("""
class $class_name: public AST {
public:
    $member_declaration;

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

class ClassTemplate:
    formatter_impl = Template("""
void $class_name::Format(std::ostream &os) const {
    os << "$class_name(";
    $code
    os << ")";
}
""")

    destructor_impl = Template("""
$class_name::~$class_name() {
    $code
}
""")

    format_item = Template(""" " $field_name=" << $expr " """)
    os_joiner = """ << os << ", " << """

    @classmethod
    def make_formatter(cls, x):
        return cls.formatter_impl.substitute(
            class_name=x.name,
            code=cls.os_joiner.join(
                cls.format_item.substitute(
                    field_name=name,
                    expr='*' + name if isinstance(type, AstNode) else name
                ) for type, name in x.members
            )
        )

    @classmethod
    def make_destructor(cls, x):
        delete_stmts = filter(None, [type.delete(name) for type, name in x.members])
        return "\n".join(delete_stmts)

    @classmethod
    def substitute_impl(cls, x):
        yield cls.make_formatter(x)
        yield cls.make_destructor(x)


class ConcreteNodeTemplate(ClassTemplate):
    decl = Template("""
class $class_name: public $base {
public:
    $member_declaration

    $class_name($constructor_args): $base($base_init), $member_init {}

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
    def substitute_impls(cls, x):
        assert isinstance(x, ConcreteNode)

    @classmethod
    def substitute(cls, x):
        assert isinstance(x, ConcreteNode)
        return cls.decl.substitute(
            class_name=x.name,
            base=x.base.name,
            member_declaration=make_member_decls(x.members),
            constructor_args=make_formal_args(x.members),
            base_init=make_actual_args(x.base.members),
            member_init=make_init(x.members),
        )


class ProductStructTempalte(ClassTemplate):
    decl = Template("""
struct $class_name: public AST {
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
        assert isinstance(x, ProductStruct)
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
                    for string, item in x.strings.items())
            )



class VisitorBaseTemplate:
    decl = Template("""
template <typename Derived>
class VisitorBase {
public:
$abstract_visitor

$concrete_visitor

};
""")

    concrete = Template("""
template<typename R, typename... Args>
R visit($class_name *node, Args&&... args) {
    return static_cast<Derived*>(this)->visit(node, args...);
}
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
    return visit(x, args...);
""")

    @classmethod
    def substitute(cls, typemap):
        pass


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
    return ", ".join("{0}({0})".format(name for _, name in members))

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

    with open(config['AST']['AST.h'], 'w') as f:
        f.write(HeaderTemplate.substitute(typemap))

    with open(config['AST']['AST.h'], 'w') as f:
        f.write(CppTemplate.substitute(typemap))

    return 0


if __name__ == '__main__':
    sys.exit(main())
