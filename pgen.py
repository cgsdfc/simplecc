import pickle
import os

from pgen2 import generate_grammar

def main():
    import argparse
    from pprint import pprint

    parser = argparse.ArgumentParser()
    parser.add_argument('--grammar', help='Grammar file', required=True)
    parser.add_argument('--dump', action='store_true', default=False,
            help='Dump grammar and exit')
    parser.add_argument('--pickle', action='store_true', default=False,
            help='Pickle the grammar for further usage')
    args = parser.parse_args()

    gr = generate_grammar(args.grammar)
    if args.dump and args.pickle:
        parser.error('Either dump or pickle, not both')

    if args.dump:
        gr.report()
    if args.pickle:
        with open(args.grammar + '.pickle', 'wb') as f:
            pickle.dump(gr, f)

if __name__ == '__main__':
    main()
