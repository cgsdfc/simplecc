#! /usr/bin/env python3
"""Driver program"""

import sys
import json
import argparse
import os
from pprint import pprint

from parser import parse_file, lispify
from cst import ToAST, astpretty_pprint
from validate import validate
from symtable import build_symtable


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('input', help='Input file to parse')
    parser.add_argument('-r', '--raw',
            dest='raw', help='print raw ast dump',
            action='store_true', default=False)
    parser.add_argument('-t', dest='dump_type',
            choices=('cst', 'ast'), default='cst')
    args = parser.parse_args()

    cst = parse_file(args.input)
    if args.dump_type == 'ast':
        ast = ToAST(cst)
        if not validate(ast):
            return 1
        symtable = build_symtable(ast)
        if symtable is None:
            return 1
        symtable.report()
        # if args.raw:
        #     print(ast)
        # else:
        #     astpretty_pprint(ast)
    else:
        if args.raw:
            print(cst)
        else:
            pprint(lispify(cst))
    return 0



if __name__ == '__main__':
    sys.exit(main())
