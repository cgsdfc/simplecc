#! /usr/bin/env python3

"""All-in-one script to generate grammar table, symbols definition for simplecompiler
while being an implementation of simplecompiler in Python.
"""

import argparse
import sys
from importlib import import_module

# artifact @ language matrix
# map to the name of module
generators = {
    'grammar': {
        'cpp': 'gencpp',
        'py': 'genpy',
    },
    'ast': {
        'cpp': 'asdl_cpp2',
        'py': 'asdl_py',
    },
}

def do_generate(args):
    mod_name = generators[args.artifact][args.language]
    module = import_module('simplecompiler.generate.{}.{}'.format(args.artifact, mod_name))
    return module.generate(args)


def do_compile(args):
    from simplecompiler.compiler import compile
    return compile(**vars(args))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(help='available commands', dest='command')
    commands.required = True

    # generate sub-command
    generate = commands.add_parser('generate', help='generate compiler a component')

    generate.add_argument('-l', '--lang',
        dest='language',
        choices=('py', 'cpp'),
        help='language of the generated artifact',
        required=True
    )

    generate.add_argument('-o', '--output',
        dest='output',
        help='output directory. write to stdout if omitted',
    )

    generate.add_argument('-d', '--dump',
        dest='dump',
        action='store_true',
        help='dump internal data structures without generating any files',
    )

    generate.add_argument('-a', '--artifact',
        dest='artifact',
        choices=('grammar', 'ast'),
        required=True,
        help='artifact to generate',
    )

    generate.add_argument('input', help='input file to the generator')
    generate.set_defaults(func=do_generate)

    # compile sub-command
    compile = commands.add_parser('compile', help='compile input file')

    compile.add_argument('input', help='input file to the compiler')

    compile.add_argument('-o', '--output',
        dest='output',
        type=argparse.FileType('w'),
        default=sys.stdout,
        help='output file (default to stdout)',
    )

    compile.add_argument('--pretty',
        dest='pretty',
        action='store_true',
        help='prettify output if true',
    )

    compile.add_argument('-p', '--phrase',
        dest='phrase',
        required=True,
        choices=(
            "tokenize",
            "parse",
            "validate",
            "build-ast",
            "build-symtable",
            "typecheck",
        ),
        help="compilation phrase to run",
    )

    compile.add_argument('--cpp-test',
        dest='cpp_test',
        action='store_true',
        help='produce cpp test based on internal data structures (symtable only)',
    )

    compile.set_defaults(func=do_compile)

    args = parser.parse_args()
    return args.func(args)
