# Copyright 2004-2005 Elemental Security, Inc. All Rights Reserved.
# Licensed to PSF under a Contributor Agreement.

import token
import tokenize
from .grammar import NFAState, DFAState


class Parser:
    """Parse Python Grammar Description

    >>> parser = Parser('Grammar/Grammar')
    >>> parser.parse()
    """

    def __init__(self, filename, stream=None):
        close_stream = None
        if stream is None:
            stream = open(filename, 'rb')
            close_stream = stream.close
        self.filename = filename
        self.stream = stream
        self.generator = tokenize.tokenize(stream.readline)
        next(self.generator)  # Eat ENCODING
        self.gettoken()  # Initialize lookahead

    def parse(self):
        dfas = {}
        startsymbol = None
        # MSTART: (NEWLINE | RULE)* ENDMARKER
        while self.type != token.ENDMARKER:
            while self.type == token.NEWLINE:
                self.gettoken()
            # RULE: NAME ':' RHS NEWLINE
            name = self.expect(token.NAME)
            self.expect(token.OP, ":")
            a, z = self.parse_rhs()
            self.expect(token.NEWLINE)
            #self.dump_nfa(name, a, z)
            dfa = self.make_dfa(a, z)
            #self.dump_dfa(name, dfa)
            oldlen = len(dfa)
            self.simplify_dfa(dfa)
            newlen = len(dfa)
            dfas[name] = dfa
            # print name, oldlen, newlen
            if startsymbol is None:
                startsymbol = name
        return dfas, startsymbol

    def make_dfa(self, start, finish):
        # To turn an NFA into a DFA, we define the states of the DFA
        # to correspond to *sets* of states of the NFA.  Then do some
        # state reduction.  Let's represent sets as dicts with 1 for
        # values.
        assert isinstance(start, NFAState)
        assert isinstance(finish, NFAState)

        def closure(state):
            base = {}
            addclosure(state, base)
            return base

        def addclosure(state, base):
            assert isinstance(state, NFAState)
            if state in base:
                return
            base[state] = 1
            for label, next in state.arcs:
                if label is None:
                    addclosure(next, base)
        states = [DFAState(closure(start), finish)]
        for state in states:  # NB states grows while we're iterating
            arcs = {}
            for nfastate in state.nfaset:
                for label, next in nfastate.arcs:
                    if label is not None:
                        addclosure(next, arcs.setdefault(label, {}))
            for label, nfaset in sorted(arcs.items()):
                for st in states:
                    if st.nfaset == nfaset:
                        break
                else:
                    st = DFAState(nfaset, finish)
                    states.append(st)
                state.addarc(st, label)
        return states  # List of DFAState instances; first one is start

    def dump_nfa(self, name, start, finish):
        print("Dump of NFA for", name)
        todo = [start]
        for i, state in enumerate(todo):
            print("  State", i, state is finish and "(final)" or "")
            for label, next in state.arcs:
                if next in todo:
                    j = todo.index(next)
                else:
                    j = len(todo)
                    todo.append(next)
                if label is None:
                    print("    -> %d" % j)
                else:
                    print("    %s -> %d" % (label, j))

    def dump_dfa(self, name, dfa):
        print("Dump of DFA for", name)
        for i, state in enumerate(dfa):
            print("  State", i, state.isfinal and "(final)" or "")
            for label, next in sorted(state.arcs.items()):
                print("    %s -> %d" % (label, dfa.index(next)))

    def simplify_dfa(self, dfa):
        # This is not theoretically optimal, but works well enough.
        # Algorithm: repeatedly look for two states that have the same
        # set of arcs (same labels pointing to the same nodes) and
        # unify them, until things stop changing.

        # dfa is a list of DFAState instances
        changes = True
        while changes:
            changes = False
            for i, state_i in enumerate(dfa):
                for j in range(i+1, len(dfa)):
                    state_j = dfa[j]
                    if state_i == state_j:
                        # print "  unify", i, j
                        del dfa[j]
                        for state in dfa:
                            state.unifystate(state_j, state_i)
                        changes = True
                        break

    def parse_rhs(self):
        # RHS: ALT ('|' ALT)*
        a, z = self.parse_alt()
        if self.value != "|":
            return a, z
        else:
            aa = NFAState()
            zz = NFAState()
            aa.addarc(a)
            z.addarc(zz)
            while self.value == "|":
                self.gettoken()
                a, z = self.parse_alt()
                aa.addarc(a)
                z.addarc(zz)
            return aa, zz

    def parse_alt(self):
        # ALT: ITEM+
        a, b = self.parse_item()
        while (self.value in ("(", "[") or
               self.type in (token.NAME, token.STRING)):
            c, d = self.parse_item()
            b.addarc(c)
            b = d
        return a, b

    def parse_item(self):
        # ITEM: '[' RHS ']' | ATOM ['+' | '*']
        if self.value == "[":
            self.gettoken()
            a, z = self.parse_rhs()
            self.expect(token.OP, "]")
            a.addarc(z)
            return a, z
        else:
            a, z = self.parse_atom()
            value = self.value
            if value not in ("+", "*"):
                return a, z
            self.gettoken()
            z.addarc(a)
            if value == "+":
                return a, z
            else:
                return a, a

    def parse_atom(self):
        # ATOM: '(' RHS ')' | NAME | STRING
        if self.value == "(":
            self.gettoken()
            a, z = self.parse_rhs()
            self.expect(token.OP, ")")
            return a, z
        elif self.type in (token.NAME, token.STRING):
            a = NFAState()
            z = NFAState()
            a.addarc(z, self.value)
            self.gettoken()
            return a, z
        else:
            self.raise_error("expected (...) or NAME or STRING, got %s/%s",
                             self.type, self.value)

    def expect(self, type, value=None):
        if self.type != type or (value is not None and self.value != value):
            self.raise_error("expected %s/%s, got %s/%s",
                             type, value, self.type, self.value)
        value = self.value
        self.gettoken()
        return value

    def gettoken(self):
        tup = next(self.generator)
        while tup[0] in (tokenize.COMMENT, tokenize.NL):
            tup = next(self.generator)
        self.type, self.value, self.begin, self.end, self.line = tup
        # print token.tok_name[self.type], repr(self.value)

    def raise_error(self, msg, *args):
        if args:
            try:
                msg = msg % args
            except:
                msg = " ".join([msg] + list(map(str, args)))
        raise SyntaxError(msg, (self.filename, self.end[0],
                                self.end[1], self.line))
