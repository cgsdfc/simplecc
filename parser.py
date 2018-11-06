# Copyright 2004-2005 Elemental Security, Inc. All Rights Reserved.
# Licensed to PSF under a Contributor Agreement.

"""Parser engine for the grammar tables generated by pgen.

The grammar table must be loaded first.

See Parser/parser.c in the Python distribution for additional info on
how this parsing engine works.

"""

import logging
from tokenizer import tokenize

logger = logging.getLogger()
logging.basicConfig(level=logging.INFO)


class ParseError(Exception):
    """Exception to signal the parser is stuck."""

    def __init__(self, msg, type, value, context):
        Exception.__init__(self, "%s: type=%r, value=%r, context=%r" %
                           (msg, type, value, context))
        self.msg = msg
        self.type = type
        self.value = value
        self.context = context


class BaseParser:
    """Parser engine."""

    def __init__(self, grammar, start=None):
        # Each stack entry is a tuple: (dfa, state, node).
        # A node is a tuple: (type, value, context, children),
        # where children is a list of nodes or None, and context may be None.

        self.grammar = grammar
        if start is None:
            start = grammar.start
        self.start = start
        newnode = (start, None, None, [])
        stackentry = (self.grammar.dfas[start], 0, newnode)
        self.stack = [stackentry]
        self.rootnode = None

    def addtoken(self, type, value, context):
        """Add a token; return True iff this is the end of the program."""
        # Map from token to label
        logger.debug('addtoken: {}'.format(str((type, value, context))))
        ilabel = self.classify(type, value, context)
        # Loop until the token is shifted; may raise exceptions
        assert self.stack, (type, value)
        while True:
            dfa, state, node = self.stack[-1]
            states, first = dfa
            arcs = states[state]
            # Look for a state with this label
            for i, newstate in arcs:
                t, v = self.grammar.labels[i]
                if ilabel == i:
                    # Look it up in the list of labels
                    assert t < 256
                    # Shift a token; we're done with it
                    self.shift(type, value, newstate, context)
                    # Pop while we are in an accept-only state
                    state = newstate
                    while states[state] == [(0, state)]:
                        self.pop()
                        if not self.stack:
                            # Done parsing!
                            return True
                        dfa, state, node = self.stack[-1]
                        states, first = dfa
                    # Done with this token
                    return False
                elif t >= 256:
                    # See if it's a symbol and if we're in its first set
                    itsdfa = self.grammar.dfas[t]
                    itsstates, itsfirst = itsdfa
                    if ilabel in itsfirst:
                        # Push a symbol
                        self.push(t, self.grammar.dfas[t], newstate, context)
                        break # To continue the outer while loop
            else:
                if (0, state) in arcs:
                    # An accepting state, pop it and try something else
                    self.pop()
                    if not self.stack:
                        # Done parsing, but another token is input
                        raise ParseError("too much input",
                                         type, value, context)
                else:
                    # No success finding a transition
                    raise ParseError("bad input", type, value, context)

    def classify(self, type, value, context):
        """Turn a token into a label.  (Internal)"""
        if value in self.grammar.keywords:
            # Check for reserved words
            ilabel = self.grammar.keywords.get(value)
            if ilabel is not None:
                return ilabel
        ilabel = self.grammar.tokens.get(type)
        if ilabel is None:
            raise ParseError("bad token", type, value, context)
        return ilabel

    def shift(self, type, value, newstate, context):
        """Shift a token.  (Internal)"""
        logger.debug('shift')
        dfa, state, node = self.stack[-1]
        newnode = (type, value, context, None)
        node[-1].append(newnode)
        self.stack[-1] = (dfa, newstate, node)

    def push(self, type, newdfa, newstate, context):
        logger.debug('push')
        """Push a nonterminal.  (Internal)"""
        dfa, state, node = self.stack[-1]
        newnode = (type, None, context, [])
        self.stack[-1] = (dfa, newstate, node)
        self.stack.append((newdfa, 0, newnode))

    def pop(self):
        logger.debug('pop')
        """Pop a nonterminal.  (Internal)"""
        *_, newnode = self.stack.pop()
        if self.stack:
            dfa, state, node = self.stack[-1]
            node[-1].append(newnode)
        else:
            self.rootnode = newnode


class Parser(BaseParser):

    def __init__(self, grammar, start=None):
        super().__init__(grammar, start)

    def parse_tokens(self, tokens):
        """Parse a series of tokens and return the syntax tree."""
        for quintuple in tokens:
            type, value, start, *_ = quintuple
            if self.addtoken(type, value, start):
                break
        else:
            raise ParseError("incomplete input", type, value, start)
        return self.rootnode

    def parse_stream(self, stream):
        """Parse a stream and return the syntax tree."""
        tokens = tokenize(stream.__next__)
        return self.parse_tokens(tokens)

    def parse_file(self, filename):
        """Parse a file and return the syntax tree."""
        with open(filename) as stream:
            return self.parse_stream(stream)

    def parse_string(self, text):
        """Parse a string and return the syntax tree."""
        tokens = tokenize(io.StringIO(text).readline)
        return self.parse_tokens(tokens)


def lispify(root, grammar):
    out = []
    type, *rest, children = root
    if type < 256:
        type = grammar.tok_name[type]
    else:
        # nonterminal
        type = grammar.number2symbol[type]
    out.append(type)
    out.extend(filter(None, rest))
    if children is not None:
        children = tuple( lispify(child, grammar) for child in children )
        out.append(children)
    return tuple(out)



def main():
    import argparse
    import os
    import pickle
    from pprint import pprint

    parser = argparse.ArgumentParser()
    parser.add_argument('input', help='Input file to parse')
    parser.add_argument('--grammar', type=argparse.FileType('rb'),
            help='Pickled grammar to use', required=True)
    args = parser.parse_args()

    with args.grammar:
        grammar = pickle.load(args.grammar)
    parser = Parser(grammar)
    rootnode = parser.parse_file(args.input)
    pprint(lispify(rootnode, grammar))

if __name__ == '__main__':
    main()
