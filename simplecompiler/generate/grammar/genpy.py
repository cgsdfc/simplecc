#! /usr/bin/env python3
import pickle
import sys

from pathlib import Path
from itertools import chain
from operator import itemgetter
from simplecompiler.generate import generate_grammar


def generate(args):
    grammar = generate_grammar(args.input)
    if args.dump:
        grammar.report()
        return 0
    if args.output is None:
        print("# generated Symbol.py")
        emit_symbol(grammar, sys.stdout)
        return 0
    outdir = Path(args.output)
    with (outdir/"Symbol.py").open('w') as f:
        emit_symbol(grammar, f)
    with (outdir/"Grammar.pickle").open('wb') as f:
        pickle.dump(grammar, f)
    return 0



def emit_symbol(gr, f):
    tok_plus_sym = sorted(chain(((tok, val) for tok, val in gr.token2id.items()
                                 if tok[0] not in ("'", '"')), gr.symbol2number.items()), key=itemgetter(1))

    for tok, val in tok_plus_sym:
        f.write("{} = {}\n".format(tok, val))
    f.write("\n")
    f.write("tok_name = {\n")
    for tok, val in tok_plus_sym:
        f.write("    {val}: {string!r},\n".format(val=val, string=tok))
    f.write("}\n")
