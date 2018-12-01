#! /usr/bin/env python3
"""Parser for simplecompiler"""
# Original copyright:
# Copyright 2004-2005 Elemental Security, Inc. All Rights Reserved.
# Licensed to PSF under a Contributor Agreement.

import logging
import pickle

from pathlib import Path
from pprint import pprint
from collections import namedtuple

from simplecompiler.compiler.Symbol import ERRORTOKEN
from simplecompiler.compiler.tokenizer import is_terminal, is_nonterminal


__all__ = ["load_grammar", "Parser", "lispify", "pprint_cst"]

logger = logging.getLogger()
logging.basicConfig(level=logging.INFO)


def load_grammar():
    """Load Grammar.pickle from simplecompiler/compiler"""
    with (Path(__file__).parent / "Grammar.pickle").open('rb') as f:
        return pickle.load(f)


# A node is a tuple: (type, value, context, children).
# where children is a list of nodes or None, and context may be None.
# context is actually the source location as a 2-tuple.
class Node(namedtuple('Node', 'type value context children')):
    "A Node in the concrete syntax tree."""

    @property
    def first_child(self):
        return self.children[0]


# Each stack entry is a tuple: (dfa, state, node).
StackEntry = namedtuple('StackEntry', 'dfa state node')


class Parser:
    """Parser engine."""

    def raise_error(self, msg, token):
        args = (self.filename, *token.start, token.line)
        raise SyntaxError(msg, args)

    def __init__(self, grammar, filename):

        self.grammar = grammar
        # for report error
        self.filename = filename
        start = grammar.start
        newnode = Node(start, None, None, [])
        stackentry = StackEntry(self.grammar.dfas[start], 0, newnode)
        self.stack = [stackentry]
        self.rootnode = None

    def addtoken(self, token):
        """Add a token; return True iff this is the end of the program."""
        logger.debug('addtoken: {}'.format(token))
        type, value, context, *_ = token
        # Map from token to label
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
                    # print("shift", self.grammar.tok_name[t], value)
                    # Look it up in the list of labels
                    assert is_terminal(t)
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
                elif is_nonterminal(t):
                    # See if it's a symbol and if we're in its first set
                    itsdfa = self.grammar.dfas[t]
                    itsstates, itsfirst = itsdfa
                    if ilabel in itsfirst:
                        # print("push", self.grammar.number2symbol[t])
                        # Push a symbol
                        self.push(t, self.grammar.dfas[t], newstate, context)
                        break  # To continue the outer while loop
            else:
                if (0, state) in arcs:
                    # An accepting state, pop it and try something else
                    self.pop()
                    if not self.stack:
                        # Done parsing, but another token is input
                        self.raise_error("too much input", token)
                else:
                    # No success finding a transition
                    self.raise_error("unexpected {!r}".format(value), token)

    def classify(self, type, value, context):
        """Turn a token into a label.  (Internal)"""
        if value in self.grammar.keywords:
            # Check for reserved words
            ilabel = self.grammar.keywords.get(value)
            if ilabel is not None:
                return ilabel
        ilabel = self.grammar.tokens.get(type)
        if ilabel is None:
            raise SyntaxError("unexpected {!r}".format(value))
            # raise ParseError("bad token", type, value, context)
        return ilabel

    def shift(self, type, value, newstate, context):
        """Shift a token.  (Internal)"""
        logger.debug('shift')
        dfa, state, node = self.stack[-1]
        newnode = Node(type, value, context, None)
        node.children.append(newnode)
        self.stack[-1] = StackEntry(dfa, newstate, node)

    def push(self, type, newdfa, newstate, context):
        """Push a nonterminal.  (Internal)"""
        logger.debug('push')
        dfa, state, node = self.stack[-1]
        newnode = Node(type, None, context, [])
        self.stack[-1] = StackEntry(dfa, newstate, node)
        self.stack.append(StackEntry(newdfa, 0, newnode))

    def pop(self):
        """Pop a nonterminal.  (Internal)"""
        # print("pop")
        *_, newnode = self.stack.pop()
        if self.stack:
            dfa, state, node = self.stack[-1]
            node.children.append(newnode)
        else:
            self.rootnode = newnode

    # public interface
    def parse_tokens(self, tokens):
        """Parse a series of tokens and return the concrete syntax tree.
        On error, SyntaxError is raised.
        """
        for token in tokens:
            if token.type == ERRORTOKEN:
                self.raise_error("bad token", token)
            if self.addtoken(token):
                break
        else:
            self.raise_error("incomplete input", token)
        return self.rootnode


def lispify(cst_node, grammar):
    """Turn cst_node into a nested tuple so that it can be
    pretty printed with proper indents.

    Note although cst_node is implemented as namedtuple, there are
    None fields within each node and they store symbol numbers rather
    than names. This function removes None fields and turns numbers into names
    so that the resulting cst_node becomes more readable.
    """
    out = []
    type, *rest, children = cst_node
    if is_terminal(type):
        type = grammar.tok_name[type]
    else:
        # nonterminal
        type = grammar.number2symbol[type]
    out.append(type)
    out.extend(filter(None, rest))
    if children is not None:
        children = tuple(lispify(child, grammar) for child in children)
        out.append(children)
    return tuple(out)


def pprint_cst(cst_node, grammar, file):
    """Pretty print an cst_node to file"""
    return pprint(lispify(cst_node, grammar), file)
