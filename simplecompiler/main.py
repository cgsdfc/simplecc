#! /usr/bin/env python3

"""All-in-one script to generate grammar table, symbols definition for simplecompiler
while being an implementation of simplecompiler in Python.
"""

import argparse
import argcomplete

def generate(args):
    pass

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(help='available commands', dest='command')

    # generate-commands
    generate = commands.add_parser('generate', help='generate various files')
    generate.add_argument('-l', '--lang', dest='language', choices=('python', 'cpp'),
            help='language of the generated artifacts')
    generate.add_argument('input', help='input file to the generator')
    generate.add_argument('-o', '--output', dest='output', help='output directory')
    artifact = generate.add_subparsers(help='artifacts that can be generated', dest='artifact')
    grammar = artifact.add_parser('grammar', help='grammar table and symbols')
    ast = artifact.add_parser('ast', help='abstract syntax tree')

    # compile-commands
    compiler = commands.add_parser('compiler', help='compile input file')
    compiler.add_argument('-o', '--output', dest='output', help='output file')
    compiler.add_argument('--pretty', dest='is_pretty', action='store_true', help='prettify output')
    phrases = compiler.add_subparsers(help='compilation phrases to execute', dest='phrase')
    tokenize = phrases.add_parser('tokenize', help='tokenize the input')
    parse = phrases.add_parser('parse', help='parse the input and generate CST')
    build_ast = phrases.add_parser('build-ast', help='build AST from CST')
    validate = phrases.add_parser('validate', help='validate the AST')
    build_symtable = phrases.add_parser('build-symtable', help='build symbol table')
    typecheck = phrases.add_parser('typecheck', help='run typecheck on the AST')
    # typecheck.set_defaults(func=typecheck)

    args = parser.parse_args()
    print(args)
    # return args.func(args)


# simplecompiler compiler tokenize 
# simplecompiler generate grammar -l python -o ./
# simplecompiler compiler parse
# simplecompiler compiler build-ast
# simplecompiler compiler build-symtable
# simplecompiler compiler typecheck

