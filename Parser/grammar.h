#ifndef GRAMMAR_H
#define GRAMMAR_H

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
  const char *symbol;
  int n_states;
  DFAState *states;
  int *first;
  int n_first;
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

#endif
