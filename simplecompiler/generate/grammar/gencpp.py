#! /usr/bin/env python3
"""Generate LL(1) parse table and token info from Grammar definition"""

from string import Template
from operator import itemgetter
from itertools import chain
from pathlib import Path

from simplecompiler.generate.grammar.pgen2 import generate_grammar
from simplecompiler.util import format_code, double_qoute


def join_ints(*ints):
    """Given a list of ints, return a underscore separated strings with them.

    >>> join_ints(1, 2, 3)
    '1_2_3'
    """
    return '_'.join(map(str, ints))


class HeaderTemplate:
    decl = Template("""
#ifndef GRAMMAR_H
#define GRAMMAR_H
#include "gramdef.h"

enum class Symbol {
$symbol_enum
};

#define NT_OFFSET $NT_OFFSET
extern Grammar CompilerGrammar;
extern const char *TokenNames[], *SymbolNames[];

#endif
""")

    enumitem = Template(""" $name = $value, """)
    NT_OFFSET = 256

    def get_all(self, gr):
        return chain(gr.symbol2number.items(), gr.token2id.items())

    def substitute(self, gr):
        return self.decl.substitute(
            symbol_enum="\n".join(self.enumitem.substitute(
                name=name,
                value=value,
            ) for name, value in self.get_all(gr)),
            NT_OFFSET=self.NT_OFFSET,
        )


class ImplTemplate:
    impl = Template("""
#include "Grammar.h"

const char *TokenNames[$n_tokens] = {
$token_names
};

const char *SymbolNames[$n_symbols] = {
$symbol_names
};

static Label labels[$n_labels] = {
$labels
};

$dfa_sections

static DFA *dfas[$n_dfas] = {
$dfa_names
};

Grammar CompilerGrammar = { $start, $n_dfas, $n_labels, labels, dfas };
""")

    arc_array = Template("""
static Arc arcs_$id[$n_arcs] = { $data };
""")
    first = Template("""
static int first_$id[$n_first] = { $data };
""")
    state_array = Template("""
static DFAState states_$id[$n_states] = { $data };
""")
    dfa_state = Template("""
{ $n_arcs, arcs_$arc_id, $is_final }
""")

    dfa = Template("""
static DFA dfa_$id = { "$name", $n_states, states_$id, $n_first, first_$id };
""")

    dfa_section = Template("""
$arc_arrays
$first_set
$state_array
$dfa_itself
""")

    pair = Template(""" { $first, $second } """)

    def make_int_pairs(self, x):
        """Fill a list of int pairs"""
        return ", ".join(self.pair.substitute(
            first=first,
            second=second,
        ) for first, second in x)

    def make_arc_array(self, arcs, i, j):
        return self.arc_array.substitute(
            id=join_ints(i, j),
            n_arcs=len(arcs),
            data=self.make_int_pairs(arcs),
        )

    def make_first(self, i, first):
        return self.first.substitute(
            id=i,
            n_first=len(first),
            data=", ".join(map(str, first)),
        )

    def make_state(self, i, j, state):
        return self.dfa_state.substitute(
            n_arcs=len(state),
            arc_id=join_ints(i, j),
            is_final=int((0, j) in state),
        )

    def make_state_array(self, i, states):
        return self.state_array.substitute(
            id=i,
            n_states=len(states),
            data=", ".join(self.make_state(i, j, state)
                           for j, state in enumerate(states)
                           )
        )

    def make_dfa(self, i, name, states, first):
        return self.dfa.substitute(
            id=i,
            name=name,
            n_states=len(states),
            n_first=len(first),
        )

    def make_dfa_section(self, i, name, states, first):
        return self.dfa_section.substitute(
            arc_arrays="\n".join(self.make_arc_array(arcs, i, j)
                                 for j, arcs in enumerate(states)),
            first_set=self.make_first(i, first),
            state_array=self.make_state_array(i, states),
            dfa_itself=self.make_dfa(i, name, states, first),
        )

    def make_dfa_sections(self, gr):
        """Create all dfa_section for all dfas in grammar"""
        def process(gr):
            """Sort the dfas by their name/rule"""
            return enumerate(sorted(gr.dfas.items(), key=itemgetter(0)))

        return "\n".join(
            self.make_dfa_section(i, gr.number2symbol[type], states, first)
            for i, (type, (states, first)) in process(gr)
        )

    def make_dfa_names(self, n_dfas):
        return ", ".join('&dfa_{}'.format(i) for i in range(n_dfas))

    def make_labels(self, labels):
        def string_or_null(v):
            return 0 if v is None else double_qoute(v)

        return ",\n".join(self.pair.substitute(
            first=symtok,
            second=string_or_null(string),
        ) for symtok, string in labels)

    def make_strings(self, x):
        """x is a mapping from string to id, return a comma-newline
        separated string for c-string in the order of ids of x.

        >>> x = {"NAME": 0, "OP": 1}

        "NAME",
        "OP",
        """
        strings = [double_qoute(s)
                   for s, _ in sorted(x.items(), key=itemgetter(1))]
        return ",\n".join(strings)

    def substitute(self, gr):
        return self.impl.substitute(
            n_tokens=len(gr.token2id),
            token_names=self.make_strings(gr.token2id),
            n_symbols=len(gr.symbol2number),
            symbol_names=self.make_strings(gr.symbol2number),
            n_labels=len(gr.labels),
            labels=self.make_labels(gr.labels),
            dfa_sections=self.make_dfa_sections(gr),
            n_dfas=len(gr.dfas),
            dfa_names=self.make_dfa_names(len(gr.dfas)),
            start=gr.start,
        )


def generate(args):
    """Entry point of main.py to this module"""
    grammar = generate_grammar(args.input)
    if args.dump:
        grammar.report()
        return 0
    header = format_code(HeaderTemplate().substitute(grammar))
    impl = format_code(ImplTemplate().substitute(grammar))
    if args.output is None:
        # no directory given, write to stdout
        print("// generated header file (Grammar.h):")
        print(header, end="\n\n")
        print("// generated impl file (Grammar.cpp):")
        print(impl)
        return 0
    outdir = Path(args.output)
    assert outdir.is_dir()
    with (outdir/"Grammar.h").open('w') as f:
        f.write(header)
    with (outdir/"Grammar.cpp").open('w') as f:
        f.write(impl)
    return 0
