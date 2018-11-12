#! /usr/bin/env python3
"""Driver program"""

import sys
import json
import argparse
import os
import pickle
from pprint import pprint

from parser import parse_file
from cst import ToAST
from validate import validate


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('input', help='Input file to parse')
    parser.add_argument('-r', '--raw',
            dest='raw', help='print raw ast dump',
            action='store_true', default=False)
    args = parser.parse_args()

    with open('./Grammar.pickle', 'rb') as f:
        grammar = pickle.load(f)

    ast = ToAST(parse_file(grammar, args.input))
    if not validate(ast):
        return 1
    if args.raw:
        print(ast)
    else:
        astpretty_pprint(ast)
    return 0



if __name__ == '__main__':
    sys.exit(main())
