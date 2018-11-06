import pickle
import os

from pgen2 import generate_grammar
from ast import literal_eval

def main():
    import argparse
    from pprint import pprint

    parser = argparse.ArgumentParser()
    parser.add_argument('--grammar', help='Grammar file', required=True)
    parser.add_argument('--dump', action='store_true', default=False,
            help='Dump grammar and exit')
    parser.add_argument('--pickle', action='store_true', default=False,
            help='Pickle the grammar for further usage')
    parser.add_argument('--tokens', help='Generate tokens.py')
    args = parser.parse_args()

    gr = generate_grammar(args.grammar)
    if args.dump and args.pickle:
        parser.error('Either dump or pickle, not both')

    if args.dump:
        gr.report()
    if args.pickle:
        with open(args.grammar + '.pickle', 'wb') as f:
            pickle.dump(gr, f)
    if args.tokens:
        with open(args.tokens, 'w') as f:
            for tok, val in gr.token2id.items():
                if tok[0] not in ("'", '"'):
                    f.write(f"{tok} = {val}\n")
            f.write("\n")
            f.write("tok_name = {\n")
            for tok, val in gr.token2id.items():
                if tok[0] in ("'", '"'):
                    tok = literal_eval(tok)
                f.write(f"    {val}: {tok!r},\n")
            f.write("}\n")


if __name__ == '__main__':
    main()
