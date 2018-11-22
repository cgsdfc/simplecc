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
#include <iostream>

enum class Opcode {
$values
};

const char *CStringFromOpcode(Opcode val);

inline std::ostream &operator<<(std::ostream &os, Opcode val) {
    return os << CStringFromOpcode(val);
}

#endif""")

    cpp = Template("""
#include "Opcode.h"

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
