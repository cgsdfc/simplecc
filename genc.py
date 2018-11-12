#! /usr/bin/env python3
from pgen2 import generate_grammar
from util import Emittor
from operator import itemgetter
from itertools import chain

Header_Header = """#ifndef GRAMMAR_H
#define GRAMMAR_H
#include "gramdef.h"

"""

Header_Trailer = """
#define NT_OFFSET 256
extern Grammar CompilerGrammar;
extern const char *TokenNames[], *SymbolNames[];

#endif
"""

Impl_Header = """#include "Grammar.h"

"""

def emit_header(config, gr):
    with open(config['Grammar']['header'], 'w') as f:
        e = Emittor(f)

        #includes
        f.write(Header_Header)

        # enum of symbols
        e.emit("enum class Symbol {")
        symbols = chain(gr.symbol2number.items(), gr.token2id.items())
        for sym, val in symbols:
            e.emit("{} = {},".format(sym, val), 1)
        e.emit("};")
        e.emit("")

        f.write(Header_Trailer)


def emit_cpp(config, gr):
    with open(config['Grammar']['cpp'], 'w') as f:
        e = Emittor(f)

        #includes
        f.write(Impl_Header)

        # TokenNames
        e.emit("const char *TokenNames[{}] = {{".format(len(gr.token2id)))
        for sym  in gr.token2id.keys():
            e.emit("\"{}\",".format(sym), 1)
        e.emit("};")
        e.emit("")

        # SymbolNames
        e.emit("const char *SymbolNames[{}] = {{".format(len(gr.symbol2number)))
        for sym in gr.symbol2number.keys():
            e.emit("\"{}\",".format(sym), 1)
        e.emit("};")
        e.emit("")

        # labels
        e.emit("static Label labels[{}] = {{".format(len(gr.labels)))
        for type, string in gr.labels:
            string = '"{}"'.format(string) if string else 0
            e.emit("{{{}, {}}},".format(type, string), 1)
        e.emit("};")
        e.emit("")

        # dfas and arcs
        type_dfas = [] # [type, dfa_name]
        for i, (type, (states, first)) in enumerate(sorted(gr.dfas.items(), key=itemgetter(0))):
            states_str = [] # content of states collected
            states_name = f"states_{i}" # the name of DFAState[] array
            for j, arcs in enumerate(states): # iterate over arcs
                n_arcs = len(arcs)
                arcs_str = ", ".join("{ %d, %d }" % arc for arc in arcs) # content of Arc[] array
                arcs_name = f"arcs_{i}_{j}" # name of Arc[] array
                e.emit(f"static Arc {arcs_name}[{n_arcs}] = {{ {arcs_str} }};")
                e.emit("")
                is_final = int((0, j) in arcs) # j is the state number, (0, state) signals accepted
                states_str.append(f"{{ {n_arcs}, {arcs_name}, {is_final} }}") # collect one DFAState

            first_name = "first_{}".format(i) # name of the first set
            e.emit("static int {name}[{count}] = {{ {data} }};".format(name=first_name,
                 count=len(first), data=", ".join(map(str, first)))) # first set
            e.emit("")
            e.emit("static DFAState {name}[{count}] = {{ {data} }};"
                    .format(name=states_name, count=len(states),
                data=", ".join(states_str))) # DFAState[] array
            e.emit("")
            dfa_name = "dfa_{}".format(i) # name of this dfa
            e.emit("static DFA {name} = {{ {symbol}, {n_states}, {states}, {first}, {n_first} }};"
                    .format(symbol='"{}"'.format(gr.number2symbol[type]),
                name=dfa_name, n_states=len(states), states=states_name,
                first=first_name, n_first=len(first)))
            type_dfas.append("&{}".format(dfa_name)) # type2dfa collected
            e.emit("")

        # type_dfas
        e.emit("static DFA *dfas[{count}] = {{ {data} }};".format(
            count=len(type_dfas), data=", ".join(type_dfas)))
        e.emit("")

        # grammar
        e.emit("Grammar CompilerGrammar = {{ {start}, {n_dfas}, {n_labels}, labels, dfas }};".format(
            start=gr.start,
            n_dfas=len(type_dfas),
            n_labels=len(gr.labels)))



def main():
    import argparse
    from pprint import pprint
    import json

    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--dump', action='store_true', default=False,
            help='Dump grammar and exit')
    parser.add_argument('-c', '--config', dest='config', type=argparse.FileType(),
            help='configure file', required=1)

    args = parser.parse_args()
    config = json.load(args.config)
    gr = generate_grammar(config['Grammar']['grammar'])

    if args.dump:
        gr.report()
        return

    if args.config:
        emit_header(config, gr)
        emit_cpp(config, gr)
        return


if __name__ == '__main__':
    main()
