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

  bool InFirst(int type) {
    for (int i = 0; i < n_first; i++)
      if (type == first[i])
        return true;
    return false;
  }

};

struct Label {
  int type;
  const char *string;
};

struct Nonterminal2DFA {
  int nonterminal;
  DFA *dfa;
};

struct Grammar {
  int start;
  std::map<int, DFA*> dfas;
  int n_labels;
  Label *labels;

  Grammar(int start, int n_dfas, int n_labels, Label *labels,
      Nonterminal2DFA *dfas): start(start), dfas(), n_labels(n_labels),
  labels(labels) {
    for (int i = 0; i < n_dfas; i++) {
      this->dfas[dfas[i].nonterminal] = dfas[i].dfa;
    }
  }
};
