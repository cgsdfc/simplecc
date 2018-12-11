#! /usr/bin/env python3

"""Ast generation"""

import sys
import os
from string import Template
from pprint import pprint
from itertools import chain
from pathlib import Path

from simplecompiler.generate import asdl
from simplecompiler.util import format_code


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

    def __init__(self, name):
        super().__init__(name)
        self.subclasses = None

    @property
    def members(self):
        """All members"""
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

    @property
    def format_members(self):
        return self.members + self.base.members


class LeafNode(AstNode):
    """Direct subclass of AST, non-virtual class"""

    def __init__(self, name, members=None):
        super().__init__(name)
        self.members = members

    @property
    def format_members(self):
        return self.members


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
    delete_opt = Template("""delete $name;""")

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
            'location': Primitive('location', False),
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


class EnumFromString:
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
            type.strings = getattr(EnumFromString, name, None)
            return

        # LeafNode
        if len(sum.types) == 1:  # direct subclass of AST -- LeafNode
            self.to_remove.append(name)  # its sum name won't be generated
            cons = sum.types[0]
            type = self.typemap[cons.name]
            assert isinstance(type, LeafNode)
            type.members = [(self.field_type(f), f.name)
                            for f in chain(cons.fields, sum.attributes)]
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
        if f.seq:
            return Sequence(type)
        if f.opt:
            return Optional(type)
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

#include "TokenInfo.h"

#include <vector>
#include <iostream>
#include <string>

namespace simplecompiler {
class AST {
public:
    virtual ~AST() = default;
    virtual const char *GetClassName() const = 0;
    virtual void Format(std::ostream &os) const = 0;
};

inline std::ostream &operator<<(std::ostream &os, const AST &ast) {
    ast.Format(os);
    return os;
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

// EnumFromString
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
}
#endif
""")

    forward_decl = Template("""class $class_name;""")

    def make_forward_decls(self, typemap):
        """Return an iterable of forward_decl strings"""
        for x in typemap.values():
            if isinstance(x, AstNode):
                yield self.forward_decl.substitute(class_name=x.name)

    def substitute(self, typemap):
        return self.decl.substitute(
            forward_declarations="\n".join(self.make_forward_decls(typemap)),
            enum_classes="\n".join(substitute_all(typemap, EnumClass)),
            abstract_classes="\n".join(substitute_all(typemap, AbstractNode)),
            concrete_classes="\n".join(substitute_all(typemap, ConcreteNode)),
            leafnode_classes="\n".join(substitute_all(typemap, LeafNode)),
            string2enum_decls="\n".join(
                EnumFromStringTempalte().substitute_decls(typemap)),
        )


def substitute_all(typemap, match, method="substitute"):
    """Substitute all types matching ``match`` and iterate over them"""
    return map(lambda x: substitute(x, method),
               filter(lambda x: isinstance(x, match), typemap.values()))


def substitute(x, method="substitute"):
    """Turn a cpp type ``x`` into substituted string"""
    obj = eval(x.__class__.__name__ + "Template()")
    return getattr(obj, method)(x)


class ImplTemplate:
    impl = Template("""
#include "AST.h"
#include <iostream>
#include <vector>
#include <cassert>

namespace simplecompiler {

// Format Optional Ast
std::ostream &operator<<(std::ostream &os, const AST *ast) {
    if (ast == nullptr)
        return os << "None";
    return os << *ast;
}

// Format Sequential Ast
template <typename T>
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

$formatter_impls
$destructor_impls
$string2enum_impls
}
""")

    def substitute(self, typemap):
        return self.impl.substitute(
            formatter_impls="\n".join(substitute_all(
                typemap,
                (ConcreteNode, LeafNode, EnumClass),
                'make_formatter',
            )),
            destructor_impls="\n".join(substitute_all(
                typemap,
                (ConcreteNode, LeafNode),
                'make_destructor',
            )),
            string2enum_impls="\n".join(
                EnumFromStringTempalte().substitute_impls(typemap)
            ),
        )


class AbstractNodeTemplate:
    decl = Template("""
class $class_name: public AST {
    int Kind;
public:
    int GetKind() const { return Kind; }
    $member_declaration

    $class_name(int Kind, $constructor_args): AST(), Kind(Kind), $member_init {}

    enum $enum_name {$enumitems};
};""")

    def substitute(self, x):
        return self.decl.substitute(
            class_name=x.name,
            member_declaration=make_member_decls(x.members),
            constructor_args=make_formal_args(x.members),
            member_init=make_init(x.members),
            enum_name=x.name + "Kind",
            enumitems=self.make_enumitems(x.subclasses),
        )

    def make_enumitems(self, subclasses):
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

    default_destructor = Template("""$class_name::~$class_name() = default;""")

    format_item = Template(""" "$field_name=" << $expr """)

    os_joiner = """ << ", " << """

    def make_formatter(self, x):
        return self.formatter_impl.substitute(
            class_name=x.name,
            code=self.os_joiner.join(
                self.format_item.substitute(
                    field_name=name,
                    expr=name,
                ) for type, name in x.format_members
            )
        )

    def make_destructor(self, x):
        delete_stmts = list(
            filter(None, [type.delete(name) for type, name in x.members]))
        if not delete_stmts: # Empty dtr
            return self.default_destructor.substitute(class_name=x.name)
        return self.destructor_impl.substitute(
            class_name=x.name,
            code="\n".join(delete_stmts)
        )

    def substitute_impl(self, x):
        yield self.make_formatter(x)
        yield self.make_destructor(x)


class ConcreteNodeTemplate(ClassImplTemplate):
    decl = Template("""
class $class_name: public $base {
public:
    $member_declaration

    $class_name($constructor_args):
        $base($base::$class_name, $base_init), $member_init {}

    ~$class_name() override;

    const char *GetClassName() const override {
        return "$class_name";
    }

    void Format(std::ostream &os) const override;

    static bool InstanceCheck($base *x) {
        return x->GetKind() == $base::$class_name;
    }
};""")

    def substitute(self, x):
        assert isinstance(x, ConcreteNode)
        return self.decl.substitute(
            class_name=x.name,
            base=x.base.name,
            member_declaration=make_member_decls(x.members),
            constructor_args=make_formal_args(x.members + x.base.members),
            base_init=make_actual_args(x.base.members),
            member_init=make_init(x.members),
        )


class LeafNodeTemplate(ClassImplTemplate):
    decl = Template("""
class $class_name: public AST {
public:
    $member_declaration

    $class_name($constructor_args): AST(), $member_init {}

    ~$class_name() override;

    const char *GetClassName() const override {
        return "$class_name";
    }

    void Format(std::ostream &os) const override;
};""")

    def substitute(self, x):
        assert isinstance(x, LeafNode)
        return self.decl.substitute(
            class_name=x.name,
            member_declaration=make_member_decls(x.members),
            constructor_args=make_formal_args(x.members),
            member_init=make_init(x.members),
        )


class EnumClassTemplate:
    decl = Template("""
enum class $class_name { $enumitems };

std::ostream &operator<<(std::ostream &os, $class_name val);
""")

    case_stmt = Template("""
case $class_name::$item: return os << "$class_name::$item";
""")

    impl = Template("""
std::ostream &operator<<(std::ostream &os, $class_name val) {
    switch (val) {
    $case_stmts
    }
}
""")

    def substitute(self, x):
        assert isinstance(x, EnumClass)
        return self.decl.substitute(
            class_name=x.name,
            enumitems=make_enumitems(x.values),
        )

    def make_formatter(self, x):
        return self.impl.substitute(
            class_name=x.name,
            case_stmts=self.make_case_stmts(x),
        )

    def make_case_stmts(self, x):
        return "\n".join(self.case_stmt.substitute(
            class_name=x.name,
            item=item) for item in x.values)


class EnumFromStringTempalte:
    decl = Template("""
$class_name $enum_from_string(const String &s);
const char *$cstring_from_enum($class_name val);
""")

    impl = Template("""
$class_name $enum_from_string(const String &s) {
    $conditions
    assert(false && "not a member of $class_name");
}

const char *$cstring_from_enum($class_name val) {
    switch (val) {
    $cases
    }
}
""")

    condition = Template("""
if (s == "$string")
    return $class_name::$item;
""")

    case = Template("""
case $class_name::$item:
    return "$string"; """)

    cstring_from_enum = lambda self, name: "CStringFrom" + name
    enum_from_string = lambda self, name: name + "FromString"

    @staticmethod
    def make_string2enums(typemap):
        """Filter the enums that needs string2enum()"""
        return filter(lambda x: isinstance(x, EnumClass) and x.strings,
                      typemap.values())

    def substitute_decls(self, typemap):
        for x in self.make_string2enums(typemap):
            yield self.decl.substitute(
                class_name=x.name,
                enum_from_string=self.enum_from_string(x.name),
                cstring_from_enum=self.cstring_from_enum(x.name),
            )

    def make_code(self, template, x):
        # condition and case are similar
        for string, item in x.strings.items():
            yield template.substitute(
                class_name=x.name,
                item=item,
                string=string
            )

    def substitute_impls(self, typemap):
        for x in self.make_string2enums(typemap):
            yield self.impl.substitute(
                enum_from_string=self.enum_from_string(x.name),
                cstring_from_enum=self.cstring_from_enum(x.name),
                class_name=x.name,
                cases="\n".join(self.make_code(self.case, x)),
                conditions="\n".join(self.make_code(self.condition, x)),
            )


class VisitorBaseTemplate:
    decl = Template("""
// Visitor Mixin that provides runtime dispatch of abstract nodes
template <typename Derived>
class VisitorBase {
public:
$abstract_visitor
};""")

    abstract = Template("""
template<typename R>
R visit$class_name($class_name *node) {
    $test_and_dispatches
    assert(false && "$class_name");
} """)

    dispatch = Template("""
if (auto x = subclass_cast<$class_name>(node)) {
    return static_cast<Derived*>(this)->visit$class_name(x);
} """)

    def make_abstract_visitor(self, x):
        assert isinstance(x, AbstractNode)
        return self.abstract.substitute(
            class_name=x.name,
            test_and_dispatches="\n".join(
                self.dispatch.substitute(class_name=sub.name) for sub in x.subclasses
            )
        )

    def substitute(self, typemap):
        return self.decl.substitute(
            abstract_visitor="\n".join(self.make_abstract_visitor(x)
                                       for x in typemap.values() if isinstance(x, AbstractNode))
        )


class ChildrenVisitorTemplate:
    decl = Template("""
/// Visitor that simply visits children recursively and return no value.
template <class Derived>
class ChildrenVisitor : public VisitorBase<Derived> {
public:
    friend class VisitorBase<Derived>;
$methods
$downcasts
};""")

    # visit children
    method = Template("""
void visit$class_name($class_name *node) {
$code
}""")

    # let subclass to customize this
    crtp_downcast = Template("""
void visit$class_name($class_name *node) {
    static_cast<Derived*>(this)->visit$class_name(node);
}""")

    # ways to visit different kinds of field: Optional, Sequence, etc.
    visit = Template("""
    visit$class_name(node->$name);
""")
    visit_for = Template("""
for (auto s: node->$name) {
    visit$class_name(s);
}""")
    visit_if = Template("""
if (node->$name) {
    visit$class_name(node->$name);
}""")

    def make_visit_children(self, members):
        for type, name in members:
            if isinstance(type, AstNode):
                yield self.visit.substitute(class_name=type.name, name=name)
            elif isinstance(type, Sequence) and isinstance(type.elemtype, AstNode):
                yield self.visit_for.substitute(
                        class_name=type.elemtype.name, name=name)
            elif isinstance(type, Optional) and isinstance(type.elemtype, AstNode):
                yield self.visit_if.substitute(
                        class_name=type.elemtype.name, name=name)

    def substitute(self, typemap):
        # methods forward to Derived
        crtp_nodes = filter(lambda t: isinstance(t, AbstractNode), typemap.values())
        # methods we implement
        impl_nodes = filter(lambda t: isinstance(t, (ConcreteNode, LeafNode)), typemap.values())
        return self.decl.substitute(
            methods="\n".join(self.method.substitute(
                class_name=t.name,
                code="\n".join(self.make_visit_children(t.members)),
            ) for t in impl_nodes),
            downcasts="\n".join(self.crtp_downcast.substitute(class_name=t.name)
                for t in crtp_nodes),
        )


class VisitorTemplate:
    decl = Template("""#ifndef VISITOR_H
#define VISITOR_H
#include "AST.h"
#include <cassert>

namespace simplecompiler {
$visitor_base
$children_visitor
}
#endif""")

    def substitute(self, typemap):
        return self.decl.substitute(
            visitor_base=VisitorBaseTemplate().substitute(typemap),
            children_visitor=ChildrenVisitorTemplate().substitute(typemap),
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


def generate(args):
    asdl_mod = asdl.parse(args.input)
    if not asdl.check(asdl_mod):
        return 1
    typemap = make_typemap(asdl_mod)
    if args.dump:
        pprint(typemap)
        return 0
    header = format_code(HeaderTemplate().substitute(typemap))
    impl = format_code(ImplTemplate().substitute(typemap))
    vis = format_code(VisitorTemplate().substitute(typemap))

    if args.output is None:
        print("// generated AST.h")
        print(header)
        print("// generated AST.cpp")
        print(impl)
        print("// generated Visitor.h")
        print(vis)
        return 0
    outdir = Path(args.output)
    with (outdir/"AST.h").open('w') as f:
        f.write(header)
    with (outdir/"AST.cpp").open('w') as f:
        f.write(impl)
    with(outdir/"Visitor.h").open('w') as f:
        f.write(vis)
    return 0
