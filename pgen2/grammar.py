# Copyright 2004-2005 Elemental Security, Inc. All Rights Reserved.
# Licensed to PSF under a Contributor Agreement.

"""This module defines the data structures used to represent a grammar.

These are a bit arcane because they are derived from the data
structures used by Python's 'pgen' parser generator.
"""


class Grammar(object):
    """Pgen parsing tables conversion class.

    Once initialized, this class supplies the grammar tables for the
    parsing engine implemented by parse.py.  The parsing engine
    accesses the instance variables directly.  The class here does not
    provide initialization of the tables; several subclasses exist to
    do this (see the conv and pgen modules).

    The instance variables are as follows:

    symbol2number -- a dict mapping symbol names to numbers.  Symbol
                     numbers are always 256 or higher, to distinguish
                     them from token numbers, which are between 0 and
                     255 (inclusive).

    number2symbol -- a dict mapping numbers to symbol names;
                     these two are each other's inverse.

    states        -- a list of DFAs, where each DFA is a list of
                     states, each state is a list of arcs, and each
                     arc is a (i, j) pair where i is a label and j is
                     a state number.  The DFA number is the index into
                     this list.  (This name is slightly confusing.)
                     Final states are represented by a special arc of
                     the form (0, j) where j is its own state number.

    dfas          -- a dict mapping symbol numbers to (DFA, first)
                     pairs, where DFA is an item from the states list
                     above, and first is a set of tokens that can
                     begin this grammar rule (represented by a dict
                     whose values are always 1).

    labels        -- a list of (x, y) pairs where x is either a token
                     number or a symbol number, and y is either None
                     or a string; the strings are keywords.  The label
                     number is the index in this list; label numbers
                     are used to mark state transitions (arcs) in the
                     DFAs.

    start         -- the number of the grammar's start symbol.

    keywords      -- a dict mapping keyword strings to arc labels.

    tokens        -- a dict mapping token numbers to arc labels.

    """

    def __init__(self):
        self.symbol2number = {}
        self.number2symbol = {}
        self.states = []
        self.dfas = {}
        self.labels = [(0, "EMPTY")]
        self.keywords = {}
        self.tokens = {}
        self.symbol2label = {}
        self.start = 256


    def report(self):
        """Dump the grammar tables to standard output, for debugging."""
        from pprint import pprint
        print("symbol2number")
        pprint(self.symbol2number)
        print("number2symbol")
        pprint(self.number2symbol)
        print("states")
        pprint(self.states)
        print("dfas")
        pprint(self.dfas)
        print("labels")
        pprint(self.labels)
        print("start", self.start)

# These classes are used by Generator

class NFAState(object):

    def __init__(self):
        self.arcs = [] # list of (label, NFAState) pairs

    def addarc(self, next, label=None):
        assert label is None or isinstance(label, str)
        assert isinstance(next, NFAState)
        self.arcs.append((label, next))

class DFAState(object):

    def __init__(self, nfaset, final):
        assert isinstance(nfaset, dict)
        assert isinstance(next(iter(nfaset)), NFAState)
        assert isinstance(final, NFAState)
        self.nfaset = nfaset
        self.isfinal = final in nfaset
        self.arcs = {} # map from label to DFAState

    def addarc(self, next, label):
        assert isinstance(label, str)
        assert label not in self.arcs
        assert isinstance(next, DFAState)
        self.arcs[label] = next

    def unifystate(self, old, new):
        for label, next in self.arcs.items():
            if next is old:
                self.arcs[label] = new

    def __eq__(self, other):
        # Equality test -- ignore the nfaset instance variable
        assert isinstance(other, DFAState)
        if self.isfinal != other.isfinal:
            return False
        # Can't just return self.arcs == other.arcs, because that
        # would invoke this method recursively, with cycles...
        if len(self.arcs) != len(other.arcs):
            return False
        for label, next in self.arcs.items():
            if next is not other.arcs.get(label):
                return False
        return True

    __hash__ = None # For Py3 compatibility.
