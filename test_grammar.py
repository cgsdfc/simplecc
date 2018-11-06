"Test whether our grammar works."
from tokenize import tokenize
import tokenize as token
import symbol

from pgen2 import generate_grammar
from driver import Driver


def main():
    import argparse
    from pprint import pprint

    parser = argparse.ArgumentParser()
    parser.add_argument('--grammar', help='Grammar file', required=True)
    parser.add_argument('--input', help='Input file to parse')
    parser.add_argument('--dump', action='store_true', default=False,
            help='Dump grammar and exit')
    args = parser.parse_args()

    gr = generate_grammar(args.grammar)
    if args.dump:
        gr.report()
        return

    assert args.input is not None
    driver = Driver(gr, gr.start)
    rootnode = driver.parse_file(args.input, "utf-8")
    pprint(rootnode)

if __name__ == '__main__':
    main()
