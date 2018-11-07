#ifndef GRAMMAR_H
#define GRAMMAR_H
#include <map>

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
  int n_states;
  DFAState *states;
  int *first;
  int n_first;

  bool InFirst(int label) {
    for (int i = 0; i < n_first; i++)
      if (label == first[i])
        return true;
    return false;
  }

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
