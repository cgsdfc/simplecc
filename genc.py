from pgen2 import generate_grammar
from util import Emittor


def emit_header(config, gr):
    with open(config['header'], 'w') as f:
        e = Emittor(f)

        #includes
        e.emit("#include \"grammar.h\"")
        e.emit("")

        # nonterminals
        for sym, val in gr.symbol2number.items():
            e.emit("#define {} {}".format(sym, val))
        e.emit("")

        # terminals
        for sym, val in gr.token2id.items():
            e.emit("#define {} {}".format(sym, val))
        e.emit("")

        e.emit("#define NT_OFFSET 256")
        e.emit("extern Grammar CompilerGrammar;")
        e.emit("extern const char *TokenNames[], *SymbolNames[];")


def emit_cpp(config, gr):
    with open(config['cpp'], 'w') as f:
        e = Emittor(f)

        #includes
        e.emit("#include \"grammar.h\"")
        e.emit("")

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
        for i, (type, (states, first)) in enumerate(gr.dfas.items()):
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
            e.emit("static int {}[{}] = {{ {} }};".format(first_name,
                 len(first), ", ".join(map(str, first)))) # first set
            e.emit("")
            e.emit("static DFAState {}[{}] = {{ {} }};".format(states_name, len(states),
                ", ".join(states_str))) # DFAState[] array
            e.emit("")
            dfa_name = "dfa_{}".format(i) # name of this dfa
            e.emit("static DFA {} = {{ {}, {}, {}, {} }};".format(
                dfa_name, len(states), states_name, first_name, len(first)))
            type_dfas.append("&{}".format(dfa_name)) # type2dfa collected
            e.emit("")

        # type_dfas
        e.emit("static DFA *dfas[{}] = {{ {} }};".format(len(type_dfas), ", ".join(type_dfas)))
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
    gr = generate_grammar(config['grammar'])

    if args.dump:
        gr.report()
        return

    if args.config:
        emit_header(config, gr)
        emit_cpp(config, gr)
        return


if __name__ == '__main__':
    main()

