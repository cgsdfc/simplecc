#include "Grammar.h"
#include <cstdio>

void DumpDFA(DFA *dfa) {
  printf("DFA(%s)\n", dfa->name);
  for (int i = 0; i < dfa->n_states; i++) {
    auto &state = dfa->states[i];
    printf("State(%d, is_final=%d)\n", i, state.is_final);

    for (int j = 0; j < state.n_arcs; j++) {
      auto &arc = state.arcs[j];
      printf("\t(%d, %d)\n", arc.label, arc.state);
    }
    printf("\n");
  }
  printf("First(");
  for (int i = 0; i < dfa->n_first; i++) {
    printf("%d,", dfa->first[i]);
  }
  printf(")\n");
}

void DumpGrammar(const Grammar &gr) {
  for (int k = 0; k < gr.n_dfas; k++) {
    auto symbol = k + NT_OFFSET;
    auto dfa = gr.dfas[k];

    printf("DFA(symbol=%d)\n", symbol);

    for (int i = 0; i < dfa->n_states; i++) {
      auto &state = dfa->states[i];
      printf("State(%d, is_final=%d):", i, state.is_final);

      for (int j = 0; j < state.n_arcs; j++) {
        auto &arc = state.arcs[j];
        printf("(%d, %d)", arc.label, arc.state);
      }
      printf("\n");
    }
    printf("First(");
    for (int i = 0; i < dfa->n_first; i++) {
      printf("%d,", dfa->first[i]);
    }
    printf(")\n\n");
  }
}
