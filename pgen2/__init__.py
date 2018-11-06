# Copyright 2004-2005 Elemental Security, Inc. All Rights Reserved.
# Licensed to PSF under a Contributor Agreement.

"""The pgen2 package."""

from .generator import Generator
from .parser import Parser

def generate_grammar(filename):
    return Generator(*Parser(filename).parse()).make_grammar()
