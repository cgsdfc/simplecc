#include "tokenize.h"
#include "grammar.h"

#include <cstring>
#include <sstream>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <memory>
#include <array>
#include <stack>

class Node {
public:
  int type;
  String value;
  std::vector<Node*> children;
  Location location;

  Node(int type, const String &value, Location location):
    type(type), value(value), children(), location(location) {}

  void AddChild(Node *child) {
    children.push_back(child);
  }
};

class StackEntry {
public:
  DFA *dfa;
  int state;
  Node *node;

  StackEntry(DFA *dfa, int state, Node *node):
    dfa(dfa), state(state), node(node) {}
};

class ParseError: public std::exception {
public:
  const char *msg;
  explicit ParseError(const char *msg): msg(msg) {}
  const char *what() const noexcept override {
    return msg;
  }
};

class Parser {
public:
  std::stack<StackEntry> stack;
  Grammar *grammar;
  int start;
  Node *rootnode;

  Parser(Grammar *grammar): stack(), grammar(grammar), start(grammar->start) {
    Node *newnode = new Node(grammar->start, "", Location());
    rootnode = nullptr;
    stack.push(StackEntry(grammar->dfas[start], 0, newnode));
  }

  int Classify(const TokenInfo &token) {
    if (token.type == NAME || token.type == OP) {
      for (int i = 0; i < grammar->n_labels; i++) {
        const char *string = grammar->labels[i].string;
        if (string && token.string == string)
          return i;
      }
    }
    for (int i = 0; i < grammar->n_labels; i++) {
      if (token.type == grammar->labels[i].type)
        return i;
    }
    return -1;
  }

  void Shift(const TokenInfo &token, int newstate) {
    StackEntry &tos = stack.top();
    tos.node->AddChild(new Node(token.type, token.string, token.start));
    tos.state = newstate;
  }

  void Push(const TokenInfo &token, DFA *newdfa, int newstate) {
    StackEntry &tos = stack.top();
    tos.state = newstate;
    Node *newnode = new Node(token.type, "", token.start);
    stack.push(StackEntry(newdfa, 0, newnode));
  }

  void Pop() {
    StackEntry &tos = stack.top();
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

    while (true) {
      StackEntry &tos = stack.top();
      DFA *dfa = tos.dfa;
      DFAState &state = dfa->states[tos.state];
      bool flag = true;

      for (int i = 0; i < state.n_arcs; ++i) {
        Arc &arc = state.arcs[i];
        int type = grammar->labels[arc.label].type;
        int newstate = arc.state;

        if (label == arc.label) {
          Shift(token, newstate);
          DFAState *states = dfa->states;

          while (states[newstate].is_final) {
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
          DFA *itsdfa = grammar->dfas[type];
          if (itsdfa->InFirst(type)) {
            Push(token, itsdfa, newstate);
            flag = false;
            break;
          }
        }
      }

      if (flag) {
        if (state.is_final) {
          Pop();
          if (stack.empty()) {
            throw ParseError("too much input");
          }
        }
        else {
          throw ParseError("bad input");
        }
      }
    }
  }

};

int main() {
  return 0;
}
