# Copyright 2004-2005 Elemental Security, Inc. All Rights Reserved.
# Licensed to PSF under a Contributor Agreement.
from ast import literal_eval
# Pgen imports
from . import grammar

class Generator(object):
    """Generate parse table"""

    def __init__(self, dfas, startsymbol):
        self.dfas = dfas
        self.startsymbol = startsymbol
        self.first = {} # map from symbol name to set of tokens
        self.addfirstsets()
        self.tokens = {} # map from token to itoken

    def make_grammar(self):
        c = grammar.Grammar()
        names = sorted(self.dfas.keys())
        names.remove(self.startsymbol)
        names.insert(0, self.startsymbol)
        for name in names:
            i = 256 + len(c.symbol2number)
            c.symbol2number[name] = i
            c.number2symbol[i] = name
        for name in names:
            dfa = self.dfas[name]
            states = []
            for state in dfa:
                arcs = []
                for label, next in sorted(state.arcs.items()):
                    arcs.append((self.make_label(c, label), dfa.index(next)))
                if state.isfinal:
                    arcs.append((0, dfa.index(state)))
                states.append(arcs)
            c.states.append(states)
            c.dfas[c.symbol2number[name]] = (states, self.make_first(c, name))
        c.start = c.symbol2number[self.startsymbol]
        c.token2id = self.tokens
        return c

    def make_first(self, c, name):
        rawfirst = self.first[name]
        first = set()
        for label in sorted(rawfirst):
            ilabel = self.make_label(c, label)
            first.add(ilabel)
        return first

    def make_token(self, label):
        if label in self.tokens:
            itoken = self.tokens[label]
        else:
            itoken = len(self.tokens)
            self.tokens[label] = itoken
        return itoken

    def make_label(self, c, label):
        ilabel = len(c.labels)
        if label[0].isalpha():
            # Either a symbol name or a named token
            if label in c.symbol2number:
                # A symbol name (a non-terminal)
                if label in c.symbol2label:
                    return c.symbol2label[label]
                else:
                    c.labels.append((c.symbol2number[label], None))
                    c.symbol2label[label] = ilabel
                    return ilabel
            else:
                # A named token (NAME, NUMBER, STRING)
                itoken = self.make_token(label)
                if itoken in c.tokens:
                    return c.tokens[itoken]
                else:
                    c.labels.append((itoken, None))
                    c.tokens[itoken] = ilabel
                    return ilabel
        else:
            # Either a keyword or an operator
            assert label[0] in ('"', "'"), label
            value = literal_eval(label)
            if value in c.keywords:
                return c.keywords[value]
            else:
                c.labels.append((self.make_token(label), value))
                c.keywords[value] = ilabel
                return ilabel

    def addfirstsets(self):
        names = list(self.dfas.keys())
        names.sort()
        for name in names:
            if name not in self.first:
                self.calcfirst(name)
            #print name, self.first[name].keys()

    def calcfirst(self, name):
        dfa = self.dfas[name]
        self.first[name] = None # dummy to detect left recursion
        state = dfa[0]
        totalset = {}
        overlapcheck = {}
        for label, next in state.arcs.items():
            if label in self.dfas:
                if label in self.first:
                    fset = self.first[label]
                    if fset is None:
                        raise ValueError("recursion for rule %r" % name)
                else:
                    self.calcfirst(label)
                    fset = self.first[label]
                totalset.update(fset)
                overlapcheck[label] = fset
            else:
                totalset[label] = 1
                overlapcheck[label] = {label: 1}
        inverse = {}
        for label, itsfirst in overlapcheck.items():
            for symbol in itsfirst:
                if symbol in inverse:
                    raise ValueError("rule %s is ambiguous; %s is in the"
                                     " first sets of %s as well as %s" %
                                     (name, symbol, label, inverse[symbol]))
                inverse[symbol] = label
        self.first[name] = totalset
