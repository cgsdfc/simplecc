import pickle
import os

from pgen2 import generate_grammar
from ast import literal_eval
from util import Emittor

GrammarDef = """

"""

def emit_header(config, gr):
    with open(config['header'], 'w') as f:
        e = Emittor(f)

        # nonterminals
        for sym, val in gr.symbol2number.items():
            e.emit("#define {} {}".format(sym, val))
        e.emit("")

        # terminals
        for sym, val in gr.token2id.items():
            e.emit("#define {} {}".format(sym, val))
        e.emit("")

        e.emit("extern const Grammar CompilerGrammar;")
        e.emit("extern const char *TokenNames[];")


def emit_cpp(config, gr):
    with open(config['cpp'], 'w') as f:
        e = Emittor(f)

        # TokenNames
        e.emit("const char *TokenNames[{}] = {{".format(len(gr.token2id)))
        for sym, val in gr.token2id.items():
            e.emit("{!r},".format(sym), 1)
        e.emit("")

        # labels
        e.emit("static const Label labels[{}] = {{".format(len(gr.labels)))
        for type, string in gr.labels:
            e.emit("{{{}, {!r}}},".format(type, string or 0), 1)
        e.emit("};")
        e.emit("")

        # dfas and arcs
        type_dfas = [] # [type, dfa_name]
        for i, (type, (first, states)) in enumerate(gr.dfas.items()):
            states_str = [] # content of states collected
            states_name = f"states_{i}" # the name of DFAState[] array
            for j, arcs in enumerate(states): # iterate over arcs
                n_arcs = len(arcs)
                arcs_str = ", ".join("{ %d, %d }" % arc for arc in arcs) # content of Arc[] array
                arcs_name = f"arcs_{i}_{j}" # name of Arc[] array
                e.emit(f"static const Arc {arcs_name}[{n_arcs}] = {{ {arcs_str} }};")
                e.emit("")
                is_final = (0, j) in arcs # j is the state number, (0, state) signals accepted
                states_str.append(f"{{ {n_arcs}, {arcs_name}, {is_final} }}") # collect one DFAState

            first_name = "first_{}".format(i) # name of the first set
            e.emit("static const int {}[{}] = {{ {} }};".format(first_name,
                 len(first), ", ".join(map(str, first)))) # firt set
            e.emit("static const DFAState {}[{}] = {{ {} }};".format(states_name, len(states),
                ", ".join(states_str))) # DFAState[] array
            e.emit("")
            dfa_name = "dfa_{}".format(i) # name of this dfa
            e.emit("static const DFA {} = {{ {}, {}, {}, {} }};".format(dfa_name, len(states), first_name,
                len(first)))
            type_dfas.append("{{ {}, &{} }}".format(type, dfa_name)) # type2dfa collected
            e.emit("")

        # type_dfas
        e.emit("static Nonterminal2DFA dfas[{}] = {{ {} }};".format(len(type_dfas),
            ", ".join(type_dfas)))
        e.emit("")

        # grammar
        e.emit("""Grammar CompilerGrammar({start}, {n_dfas}, {n_labels},
        &labels, &dfas);""".format(
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
            help='configure file', required=True)
    args = parser.parse_args()
    config = json.load(args.config)
    gr = generate_grammar(config['grammar'])

    if args.dump:
        gr.report()
        return

    emit_header(config, gr)
    emit_cpp(config, gr)


if __name__ == '__main__':
    main()
