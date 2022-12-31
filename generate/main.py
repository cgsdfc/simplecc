# MIT License

# Copyright (c) 2018 Cong Feng.

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

"""
Generate grammar table, symbols definition for simplecompiler.
"""

import argparse
from importlib import import_module

# artifact @ language matrix
# map to the name of module
generators = {
    'grammar': {
        'cpp': 'gencpp',
    },
    'ast': {
        'cpp': 'asdl_cpp2',
    },
}


def do_generate(args):
    mod_name = generators[args.artifact][args.language]
    module = import_module('simplecompiler.generate.{}.{}'.format(args.artifact, mod_name))
    return module.generate(args)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    generate = parser

    generate.add_argument('-o', '--output',
                          dest='output',
                          help='output directory. write to stdout if omitted',
                          )

    generate.add_argument('-d', '--dump',
                          dest='dump',
                          action='store_true',
                          help='dump internal data structures without generating any files',
                          )

    generate.add_argument('-a', '--artifact',
                          dest='artifact',
                          choices=('grammar', 'ast',),
                          required=True,
                          help='artifact to generate',
                          )

    generate.add_argument('input', help='input file to the generator')
    generate.set_defaults(func=do_generate)

    args = parser.parse_args()
    return args.func(args)
