#include "tokenize.h"

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

  void Format(std::FILE *file) {
    const char *type_str = GetSymName(type);

    fprintf(file, "(%s", type_str);
    if (value.size()) {
      fprintf(file, ", %s", value.c_str());
    }
    fprintf(file, ", (\n");

    if (children.size()) {
      for (auto child: children) {
        child->Format(file);
        fprintf(file, ", ");
      }
    }
    fprintf(file, ")");
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

class BaseParser {
public:
  std::stack<StackEntry> stack;
  Grammar *grammar;
  int start;
  Node *rootnode;

  explicit BaseParser(Grammar *grammar): stack(), grammar(grammar),
  start(grammar->start) {
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

class Parser: public BaseParser {
public:
  Parser(): BaseParser(&CompilerGrammar) {}

  Node *Parse(const TokenBuffer &tokens) {
    for (auto token: tokens) {
      if (token->type == ERRORTOKEN) {
        std::fprintf(stderr, "error token %s at %s\n",
            token->string.c_str(), token->start.ToString().c_str());
        return nullptr;
      }
      if (AddToken(*token)) {
        return rootnode;
      }
    }
    throw ParseError("incomplete input");
  }
};

int main(int argc, char **argv) {
  TokenBuffer tokens;
  if (argc == 1) {
    Tokenize(std::cin, tokens);
  } else if (argc == 2) {
    std::ifstream ifs(argv[1]);
    if (ifs.fail()) {
      fprintf(stderr, "file %s not exist\n", argv[1]);
      exit(1);
    }
    Tokenize(ifs, tokens);
  } else {
    fputs("Usage: parser [file]\n", stderr);
    exit(1);
  }

  Parser parser;
  try {
    Node *root = parser.Parse(tokens);
    root->Format(stdout);
  } catch (ParseError &e) {
    fprintf(stderr, "%s\n", e.what());
  }

  for (auto token: tokens) {
    delete token;
  }
  return 0;
}
