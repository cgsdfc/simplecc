#! /usr/bin/env python3

"""All-in-one script to generate grammar table, symbols definition for simplecompiler
while being an implementation of simplecompiler in Python.
"""

import argparse

def do_generate(args):
    assert args.command == 'generate'
    if args.artifact == 'grammar':
        if args.language == 'cpp':
            from simplecompiler.generate.grammar.gencpp import generate
        else:
            assert args.language == 'python'
            from simplecompiler.generate.grammar.genpy import generate
    else:
        assert args.artifact == 'ast'
        if args.language == 'cpp':
            from simplecompiler.generate.ast.asdl_cpp2 import generate
        else:
            assert args.language == 'python'
            from simplecompiler.generate.ast.asdl_py import generate
    return generate(args)


def compiler(args):
    pass

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(help='available commands', dest='command')
    commands.required = True

    # generate-commands
    generate = commands.add_parser('generate', help='generate various files')
    generate.add_argument('-l', '--lang', dest='language', choices=('python', 'cpp'),
            help='language of the generated artifacts', required=True)
    generate.add_argument('-o', '--output', dest='output', help='output directory. write to stdout if omitted')
    generate.add_argument('-d', '--dump', dest='dump', action='store_true',
            help='dump internal data structures without generating any files')
    generate.set_defaults(func=do_generate)

    artifact = generate.add_subparsers(help='artifacts that can be generated', dest='artifact')
    artifact.required = True
    grammar = artifact.add_parser('grammar', help='grammar table and symbols')
    ast = artifact.add_parser('ast', help='abstract syntax tree')
    generate.add_argument('input', help='input file to the generator')

    # compile-commands
    compiler = commands.add_parser('compiler', help='compile input file')
    compiler.add_argument('-o', '--output', dest='output', help='output file')
    compiler.add_argument('--pretty', dest='is_pretty', action='store_true', help='prettify output')
    phrases = compiler.add_subparsers(help='compilation phrases to execute', dest='phrase')
    phrases.required = True

    tokenize = phrases.add_parser('tokenize', help='tokenize the input')
    parse = phrases.add_parser('parse', help='parse the input and generate CST')
    build_ast = phrases.add_parser('build-ast', help='build AST from CST')
    validate = phrases.add_parser('validate', help='validate the AST')
    build_symtable = phrases.add_parser('build-symtable', help='build symbol table')
    typecheck = phrases.add_parser('typecheck', help='run typecheck on the AST')
    # typecheck.set_defaults(func=typecheck)

    args = parser.parse_args()
    print(args)
    if args.command is None:
        parser.error("one command must be given")
    if args.command == 'generate':
        if args.artifact is None:
            parser.error("one artifact must be given")
    if args.command == 'compiler':
        if args.phrase is None:
            parser.error("one phrase must be given")
    return args.func(args)
