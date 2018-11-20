#! /usr/bin/env python3
from pgen2 import generate_grammar


def emit_symbol(config, gr):
    from itertools import chain
    from operator import itemgetter

    tok_plus_sym = sorted(chain(((tok, val) for tok, val in gr.token2id.items()
                                 if tok[0] not in ("'", '"')), gr.symbol2number.items()), key=itemgetter(1))

    with open(config['Grammar']['symbol'], 'w') as f:
        for tok, val in tok_plus_sym:
            f.write("{} = {}\n".format(tok, val))
        f.write("\n")
        f.write("tok_name = {\n")
        for tok, val in tok_plus_sym:
            f.write("    {val}: {string!r},\n".format(val=val, string=tok))
        f.write("}\n")


def emit_pickle(config, gr):
    import pickle
    with open(config['Grammar']['pickle'], 'wb') as f:
        pickle.dump(gr, f)


def main():
    import argparse
    from pprint import pprint
    import json

    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--dump', action='store_true', default=False,
                        help='Dump grammar and exit')
    parser.add_argument('-c', '--config', dest='config', type=argparse.FileType(),
                        help='configure file', required=1)

    args = parser.parse_args()
    config = json.load(args.config)
    gr = generate_grammar(config['Grammar']['grammar'])

    if args.dump:
        gr.report()
        return

    if args.config:
        emit_symbol(config, gr)
        emit_pickle(config, gr)


if __name__ == '__main__':
    main()
