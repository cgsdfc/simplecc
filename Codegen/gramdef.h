#ifndef GRAMDEF_H
#define GRAMDEF_H

struct Arc {
  int label;
  int state;
};

struct DFAState {
  int n_arcs;
  Arc *arcs;
  bool is_final;
};

struct DFA {
  const char *name;
  int n_states;
  DFAState *states;
  int n_first;
  int *first;
};

struct Label {
  int type;
  const char *string;
};

struct Grammar {
  int start;
  int n_dfas;
  int n_labels;
  Label *labels;
  DFA **dfas;
};

void DumpDFA(DFA *dfa);
void DumpGrammar(const Grammar &gr);

#endif
