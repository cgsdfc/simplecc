#! /usr/bin/env python3
"""Cross-validate cst generation"""

import os
import tempfile

def run_test(config):
    cparser = os.path.abspath(config['cparser'])
    pyparser = os.path.abspath(config['pyparser'])
    input = os.path.abspath(config['input'])

    cparser_out = tempfile.mkstemp(prefix='cpp')[1]
    pyparser_out = tempfile.mkstemp(prefix='py')[1]

    cparser = "{exe} {input} >{tmp}".format(exe=cparser, input=input, tmp=cparser_out)
    pyparser = "{exe} {input} -r >{tmp}".format(exe=pyparser, input=input, tmp=pyparser_out)
    os.system(cparser)
    os.system(pyparser)
    os.system("cmp {a} {b}".format(a=cparser_out, b=pyparser_out))

    os.remove(cparser_out)
    os.remove(pyparser_out)


def main():
    import argparse
    import json
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--config', dest='config', type=argparse.FileType(),
            help='configure file', required=1)
    parser.add_argument('-i', dest='input', help='Override input in config')
    args = parser.parse_args()

    config = json.load(args.config)
    if args.input:
        config['input'] = args.input
    run_test(config)

if __name__ == '__main__':
    main()
