#! /usr/bin/env python3

"""All-in-one script to generate grammar table, symbols definition for simplecompiler
while being an implementation of simplecompiler in Python.
"""

import argparse
from importlib import import_module

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
    print(module)
    return module.generate(args)


def compiler(args):
    pass

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

    generate.add_argument('artifact',
        choices=('grammar', 'ast'),
        help='artifact to generate'
    )

    generate.add_argument('input', help='input file to the generator')
    generate.set_defaults(func=do_generate)

    # compiler sub-command
    compiler = commands.add_parser('compiler', help='compile input file')
    compiler.add_argument('-o', '--output', dest='output', help='output file')
    compiler.add_argument('--pretty', dest='is_pretty', action='store_true', help='prettify output')
    phrases = compiler.add_subparsers(help='compilation phrases to execute', dest='phrase')
    phrases.required = True

    # phrases sub-sub-commands
    tokenize = phrases.add_parser('tokenize', help='tokenize the input')
    parse = phrases.add_parser('parse', help='parse the input and generate CST')
    build_ast = phrases.add_parser('build-ast', help='build AST from CST')
    validate = phrases.add_parser('validate', help='validate the AST')
    build_symtable = phrases.add_parser('build-symtable', help='build symbol table')
    typecheck = phrases.add_parser('typecheck', help='run typecheck on the AST')
    # typecheck.set_defaults(func=typecheck)

    args = parser.parse_args()
    print(args)
    return args.func(args)
