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
Opcode MakeStore(Scope scope);
Opcode MakeLoad(Scope scope);
Opcode MakeRead(BasicTypeKind type);
Opcode MakePrint(BasicTypeKind type);
Opcode MakeBinary(OperatorKind oper);
Opcode MakeUnary(UnaryopKind oper);
inline std::ostream &operator<<(std::ostream &os, Opcode val) {
    return os << CStringFromOpcode(val);
}

#endif""")

    cpp = Template("""
#include "Opcode.h"

// Hard coded mappings
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
  case BasicTypeKind::Void:
    assert(false);
  }
}

Opcode MakePrint(BasicTypeKind type) {
  switch (type) {
  case BasicTypeKind::Character:
    return Opcode::PRINT_CHARACTER;
  case BasicTypeKind::Int:
    return Opcode::PRINT_INTEGER;
  default:
    assert(false);
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
  default:
    assert(false);
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

const char *CStringFromOpcode(Opcode val) {
    switch (val) {
    $code
    }
}""")

    def __init__(self, values):
        self.values = values

    def substitute_cpp(self):
        return self.cpp.substitute(code="\n".join(
            Template("""case Opcode::$val: return "Opcode::$val";""").substitute(val=val)
            for val in self.values)
        )

    def substitute_header(self):
        return self.header.substitute(values=",\n".join(self.values))


def generate(args):
    values = extract_opcode(args.input)
    code = OpcodeTemplate(values)
    header = format_code(code.substitute_header())
    cpp = format_code(code.substitute_cpp())
    if args.output is None:
        print(header)
        print(cpp)
    else:
        with open(Path(args.output)/"Opcode.h", 'w') as f:
            f.write(header)
        with open(Path(args.output)/"Opcode.cpp", 'w') as f:
            f.write(cpp)
    return 0
