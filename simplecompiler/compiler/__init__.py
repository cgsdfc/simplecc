from simplecompiler.compiler.tokenizer import *
from simplecompiler.compiler.parser import *
from simplecompiler.compiler.symtable import *
from simplecompiler.compiler.cst import *
from simplecompiler.compiler.validate import *


def compile(*, input, output, phrase, pretty, **kwargs):
    """Multiple phrases compilation"""

    with open(input) as file:
        tokens = tokenize(file)
        if phrase == 'tokenize':
            print_tokens(tokens, output)
            return 0

        grammar = load_grammar()
        parser = Parser(grammar, input)
        cst_node = parser.parse_tokens(tokens)
        if phrase == 'parse':
            if pretty:
                pprint_cst(cst_node, grammar, file=output)
            else:
                print(cst_node, file=output)
            return 0

        ast_node = build_ast(cst_node)
        if phrase == 'build-ast':
            if pretty:
                astpretty_pprint(ast_node, file=output)
            else:
                print(ast_node, file=output)
            return 0

        ok = validate(ast_node)
        if not ok or phrase == 'validate':
            return int(not ok)

        symtable = build_symtable(ast_node)
        if symtable is None:
            return 1
        if phrase == 'build-symtable':
            if kwargs['cpp_test']:
                symtable_make_test(symtable, output)
            else:
                symtable.report(output)
            return 0
        raise NotImplementedError("phrase {!r}".format(phrase))
