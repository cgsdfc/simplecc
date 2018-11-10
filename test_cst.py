#! /usr/bin/env python3
"""Cross-validate cst generation"""

import logging
import os
import tempfile

def run_test(config, input):

    cparser = os.path.abspath(config['cparser'])
    pyparser = os.path.abspath(config['pyparser'])
    input = os.path.abspath(input)

    logging.info("input: {}".format(input))
    cparser_out = tempfile.mkstemp(prefix='cpp')[1]
    pyparser_out = tempfile.mkstemp(prefix='py')[1]

    cparser = "{exe} {input} >{tmp}".format(exe=cparser, input=input, tmp=cparser_out)
    pyparser = "{exe} {input} -r >{tmp}".format(exe=pyparser, input=input, tmp=pyparser_out)

    logging.info("run: {}".format(cparser))
    os.system(cparser)

    logging.info("run: {}".format(pyparser))
    os.system(pyparser)
    ret = os.system("cmp {a} {b}".format(a=cparser_out, b=pyparser_out))
    if ret == 0:
        logging.info("ok")
    else:
        logging.info("fail")

    os.remove(cparser_out)
    os.remove(pyparser_out)


def main():
    import argparse
    import json
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--config', dest='config', type=argparse.FileType(),
            help='configure file', required=1)
    parser.add_argument('-i', dest='inputs', nargs='+', type=str, help='input files', required=1)
    parser.add_argument('-v', '--verbose', dest='verbose', action='store_true',
            default=False)
    args = parser.parse_args()

    config = json.load(args.config)
    if args.verbose:
        logging.basicConfig(level=logging.INFO)

    for input in args.inputs:
        run_test(config, input)

if __name__ == '__main__':
    main()
