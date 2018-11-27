#! /usr/bin/env python3
import sys
from string import Template
from pprint import pprint
from enum import Enum
from operator import attrgetter
from tokenize import tokenize, NAME
from pathlib import Path
from simplecompiler.util import format_code


def extract_opcode(filename):
    with open(filename, 'rb') as f:
        tokens = tokenize(f.__next__)
        # all upper case names
        names = filter(lambda t: t.type == NAME and t.string.isupper(), tokens)
        opcodes = map(attrgetter('string'), names)
        return list(opcodes)


class OpcodeTemplate:
    header = Template("""
#ifndef OPCODE_H
#define OPCODE_H
#include "AST.h"
#include "symtable.h"
#include <iostream>

enum class Opcode {
$values
};

const char *CStringFromOpcode(Opcode val);
Opcode MakeSubScr(ExprContextKind ctx);
Opcode MakeStore(Scope scope);
Opcode MakeLoad(Scope scope);
Opcode MakeRead(BasicTypeKind type);
Opcode MakePrint(BasicTypeKind type);
Opcode MakeBinary(OperatorKind oper);
Opcode MakeUnary(UnaryopKind oper);
Opcode MakeJump(OperatorKind oper);
Opcode MakeJumpNegative(OperatorKind oper);
inline std::ostream &operator<<(std::ostream &os, Opcode val) {
    return os << CStringFromOpcode(val);
}

#endif""")

    cpp = Template("""
#include "Opcode.h"

// Hard coded mappings

Opcode MakeSubScr(ExprContextKind ctx) {
    switch (ctx) {
    case ExprContextKind::Load:
        return Opcode::BINARY_SUBSCR;
    case ExprContextKind::Store:
        return Opcode::STORE_SUBSCR;
    }
}

Opcode MakeLoad(Scope scope) {
    switch (scope) {
    case Scope::Global:
      return Opcode::LOAD_GLOBAL;
    case Scope::Local:
      return Opcode::LOAD_LOCAL;
    }
}

Opcode MakeStore(Scope scope) {
    switch (scope) {
    case Scope::Global:
      return Opcode::STORE_GLOBAL;
    case Scope::Local:
      return Opcode::STORE_LOCAL;
    }
}

Opcode MakeRead(BasicTypeKind type) {
  switch (type) {
  case BasicTypeKind::Character:
    return Opcode::READ_CHARACTER;
  case BasicTypeKind::Int:
    return Opcode::READ_INTEGER;
  $default
  }
}

Opcode MakePrint(BasicTypeKind type) {
  switch (type) {
  case BasicTypeKind::Character:
    return Opcode::PRINT_CHARACTER;
  case BasicTypeKind::Int:
    return Opcode::PRINT_INTEGER;
  $default
  }
}

Opcode MakeBinary(OperatorKind oper) {
  switch (oper) {
  case OperatorKind::Add:
    return Opcode::BINARY_ADD;
  case OperatorKind::Sub:
    return Opcode::BINARY_SUB;
  case OperatorKind::Mult:
    return Opcode::BINARY_MULTIPLY;
  case OperatorKind::Div:
    return Opcode::BINARY_DIVIDE;
  $default
  }
}

Opcode MakeUnary(UnaryopKind oper) {
    switch (oper) {
    case UnaryopKind::UAdd:
        return Opcode::UNARY_POSITIVE;
    case UnaryopKind::USub:
        return Opcode::UNARY_NEGATIVE;
    }
}

Opcode MakeJumpNegative(OperatorKind oper) {
    switch (oper) {
    case OperatorKind::NotEq:
        return Opcode::JUMP_IF_EQUAL;
    case OperatorKind::Eq:
        return Opcode::JUMP_IF_NOT_EQUAL;
    case OperatorKind::GtE:
        return Opcode::JUMP_IF_LESS;
    case OperatorKind::Gt:
        return Opcode::JUMP_IF_LESS_EQUAL;
    case OperatorKind::LtE:
        return Opcode::JUMP_IF_GREATER;
    case OperatorKind::Lt:
        return Opcode::JUMP_IF_GREATER_EQUAL;
    $default
    }
}

Opcode MakeJump(OperatorKind oper) {
    switch (oper) {
    case OperatorKind::Eq:
        return Opcode::JUMP_IF_EQUAL;
    case OperatorKind::NotEq:
        return Opcode::JUMP_IF_NOT_EQUAL;
    case OperatorKind::Lt:
        return Opcode::JUMP_IF_LESS;
    case OperatorKind::LtE:
        return Opcode::JUMP_IF_LESS_EQUAL;
    case OperatorKind::Gt:
        return Opcode::JUMP_IF_GREATER;
    case OperatorKind::GtE:
        return Opcode::JUMP_IF_GREATER_EQUAL;
    $default
    }
}

const char *CStringFromOpcode(Opcode val) {
    switch (val) {
    $code
    }
}""")

    default = "default: assert(false);"

    def __init__(self, values):
        self.values = values

    def substitute_cpp(self):
        return self.cpp.substitute(
                code="\n".join(Template("""case Opcode::$val: return "Opcode::$val";""")
                    .substitute(val=val) for val in self.values),
                default=self.default,
        )

    def substitute_header(self):
        return self.header.substitute(values=",\n".join(self.values))


class OpcodeDispatcherTemplate:
    template = Template("""
#ifndef OPCODE_DISPATCHER
#define OPCODE_DISPATCHER
#include "ByteCode.h"

template <class Derived>
class OpcodeDispatcher {
public:
  void dispatch(const ByteCode &byteCode) {
    switch (byteCode.GetOpcode()) {
        $handlers
    }
  }
};

#endif""")

    handler = Template("""
case Opcode::$item:
  return static_cast<Derived*>(this)->$method_name(byteCode);
""")

    skeleton = Template("""
void $name(const ByteCode &code) {

}""")

    @staticmethod
    def make_method_name(value):
        return "Handle" + "".join(s.capitalize() for s in value.split('_'))

    def substitute_skeleton(self, values):
        return "\n".join(self.skeleton.substitute(
            name=self.make_method_name(value))
            for value in values
        )

    def substitute(self, values):
        return self.template.substitute(handlers="".join(
            self.handler.substitute(item=value,
                method_name=self.make_method_name(value))
            for value in values)
        )


def generate(args):
    values = extract_opcode(args.input)
    code = OpcodeTemplate(values)
    header = format_code(code.substitute_header())
    cpp = format_code(code.substitute_cpp())
    dispatch = format_code(OpcodeDispatcherTemplate().substitute(values))
    skeleton = OpcodeDispatcherTemplate().substitute_skeleton(values)

    if args.output is None:
        print(header)
        print(cpp)
        print(dispatch)
        print(skeleton)
    else:
        with open(Path(args.output)/"Opcode.h", 'w') as f:
            f.write(header)
        with open(Path(args.output)/"OpcodeDispatcher.h", 'w') as f:
            f.write(dispatch)
        with open(Path(args.output)/"Skeleton.h", 'w') as f:
            f.write(skeleton)
        with open(Path(args.output)/"Opcode.cpp", 'w') as f:
            f.write(cpp)
    return 0
