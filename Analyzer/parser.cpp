#include "parser.h"

#include <sstream>
#include <vector>
#include <cstdio>
#include <stack>
#include <exception>

class StackEntry {
public:
  DFA *dfa;
  int state;
  Node *node;

  StackEntry(DFA *dfa, int state, Node *node):
    dfa(dfa), state(state), node(node) {}
};

class Exception: public std::exception {
public:
  const char *msg;
  explicit Exception(const char *msg): msg(msg) {}
  const char *what() const noexcept override {
    return msg;
  }
};

class ParseError: public std::exception {
public:
  const char *msg;
  explicit ParseError(const char *msg): msg(msg) {}
  const char *what() const noexcept override {
    return msg;
  }
};

bool IsInFirst(DFA *dfa, int label) {
  for (int i = 0; i < dfa->n_first; i++)
    if (label == dfa->first[i])
      return true;
  return false;
}

bool IsAcceptOnlyState(DFAState *state) {
  return state->is_final && state->n_arcs == 1;
}

void DumpDFA(DFA *dfa) {
  printf("DFA(%s)\n", dfa->symbol);
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

void DumpStackEntry(StackEntry e) {
  printf("state: %d\n", e.state);
  printf("dfa: %s\n", e.dfa->symbol);
  /* DumpDFA(e.dfa); */
}

class BaseParser {
public:
  std::stack<StackEntry> stack;
  Grammar *grammar;
  int start;
  Node *rootnode;

  explicit BaseParser(Grammar *grammar): stack(), grammar(grammar),
  start(grammar->start) {
    Node *newnode = new Node(static_cast<Symbol>(grammar->start), "", Location());
    rootnode = nullptr;
    stack.push(StackEntry(grammar->dfas[start - NT_OFFSET], 0, newnode));
  }

  int Classify(const TokenInfo &token) {
    if (token.type == Symbol::NAME || token.type == Symbol::OP) {
      for (int i = 1; i < grammar->n_labels; i++) {
        const Label &l = grammar->labels[i];
        if (l.string && l.string == token.string)
          return i;
      }
    }
    for (int i = 1; i < grammar->n_labels; i++) {
      const Label &l = grammar->labels[i];
      if (l.type == static_cast<int>(token.type) && l.string == nullptr) {
        return i;
      }
    }
    throw ParseError("bad token");
  }

  void Shift(const TokenInfo &token, int newstate) {
    /* printf("shift %s\n", GetSymName(token.type)); */
    StackEntry &tos = stack.top();
    tos.node->AddChild(new Node(token.type, token.string, token.start));
    tos.state = newstate;
  }

  void Push(int type, DFA *newdfa, int newstate, Location location) {
    /* printf("push %s\n", GetSymName(type)); */
    StackEntry &tos = stack.top();
    Node *newnode = new Node(static_cast<Symbol>(type), "", location);
    tos.state = newstate;
    stack.push(StackEntry(newdfa, 0, newnode));
  }

  void Pop() {
    /* printf("pop\n"); */
    StackEntry tos = stack.top();
    stack.pop();
    Node *newnode = tos.node;

    if (stack.size()) {
      stack.top().node->AddChild(newnode);
    }
    else {
      rootnode = newnode;
    }
  }

  bool AddToken(const TokenInfo &token) {
    int label = Classify(token);
    /* token.Format(stdout); */

    while (true) {
      StackEntry &tos = stack.top();
      DFA *dfa = tos.dfa;
      DFAState *states = dfa->states;
      DFAState *state = &states[tos.state];
      bool flag = true;

      /* DumpStackEntry(tos); */

      for (int i = 0; i < state->n_arcs; ++i) {
        Arc &arc = state->arcs[i];
        int type = grammar->labels[arc.label].type;
        int newstate = arc.state;

        if (label == arc.label) {
          Shift(token, newstate);
          /* printf("shift %s\n", GetSymName(type)); */

          while (IsAcceptOnlyState(&states[newstate])) {
            /* printf("pop\n"); */
            Pop();
            if (stack.empty()) {
              return true;
            }
            newstate = stack.top().state;
            states = stack.top().dfa->states;
          }
          return false;
        }

        else if (type >= 256) {
          DFA *itsdfa = grammar->dfas[type - NT_OFFSET];
          if (IsInFirst(itsdfa, label)) {
            /* printf("push %s\n", GetSymName(type)); */
            Push(type, itsdfa, newstate, token.start);
            flag = false;
            break;
          }
        }
      }

      if (flag) {
        if (state->is_final) {
          Pop();
          if (stack.empty()) {
            throw ParseError("too much input");
          }
        }
        else {
          /* DumpDFA(dfa); */
          /* printf("tos.state: %d\n", tos.state); */
          /* printf("label: %d\n", label); */
          /* printf("token: %d\n", token.type); */
          throw ParseError("bad input");
        }
      }
    }
  }

};

void DumpGrammar(const Grammar &gr)  {
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


Node *ParseTokens(const TokenBuffer &tokens) {
  BaseParser parser(&CompilerGrammar);

  for (auto token: tokens) {
    if (token->type == Symbol::ERRORTOKEN) {
      std::fprintf(stderr, "error token %s at %s\n",
          token->string.c_str(), token->start.ToString().c_str());
      return nullptr;
    }
    if (parser.AddToken(*token)) {
      return parser.rootnode;
    }
  }
  throw ParseError("incomplete input");
}
