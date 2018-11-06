"Grammar Generator"

from pgen2 import generate_grammar
from pgen2.python_parser import Parser
from pgen2.driver import Driver


def main():
    import argparse
    from pprint import pprint

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('grammar', help='Grammar file')
    args = parser.parse_args()

    gr = generate_grammar(args.grammar)
    pprint(gr.report())

if __name__ == '__main__':
    main()
