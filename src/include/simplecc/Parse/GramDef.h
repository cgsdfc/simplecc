#ifndef SIMPLECC_PARSE_GRAMDEF_H
#define SIMPLECC_PARSE_GRAMDEF_H
namespace simplecc {
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
} // namespace simplecc
#endif