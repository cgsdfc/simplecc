#include "parser.h"
#include "Node.h"
#include "error.h"

#include <algorithm>
#include <cstdio>
#include <sstream>
#include <stack>
#include <vector>

using namespace simplecompiler;

namespace {
struct StackEntry {
  DFA *dfa;
  int state;
  Node *node;

  StackEntry(DFA *dfa, int state, Node *node)
      : dfa(dfa), state(state), node(node) {}

  void Dump() const {
    printf("state: %d\n", state);
    printf("dfa: %s\n", dfa->name);
  }
};

class Parser {
  std::stack<StackEntry> stack;
  Grammar *grammar;
  Node *rootnode;
  ErrorManager e;

  int Classify(const TokenInfo &token);
  void Shift(const TokenInfo &token, int newstate);
  void Push(Symbol type, DFA *newdfa, int newstate, const Location &location);
  void Pop();
  bool AddToken(const TokenInfo &token);

  static bool IsInFirst(DFA *dfa, int label) {
    auto end = dfa->first + dfa->n_first;
    return std::find(dfa->first, end, label) != end;
  }

  static bool IsAcceptOnlyState(DFAState *state) {
    return state->is_final && state->n_arcs == 1;
  }

public:
  explicit Parser(Grammar *grammar);
  Node *ParseTokens(const TokenBuffer &tokens);
};

} // namespace

Parser::Parser(Grammar *grammar)
    : stack(), grammar(grammar), rootnode(nullptr), e() {
  auto start = grammar->start;
  Node *newnode = new Node(static_cast<Symbol>(start), "", Location(0, 0));
  stack.push(StackEntry(grammar->dfas[start - NT_OFFSET], 0, newnode));
}

int Parser::Classify(const TokenInfo &token) {
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
  e.SyntaxError(token.start, "unexpected", Quote(token.string));
  return -1;
}

void Parser::Shift(const TokenInfo &token, int newstate) {
  /* printf("shift %s\n", GetSymName(token.type)); */
  StackEntry &tos = stack.top();
  tos.node->AddChild(new Node(token.type, token.string, token.start));
  tos.state = newstate;
}

void Parser::Push(Symbol type, DFA *newdfa, int newstate,
                  const Location &location) {
  /* printf("push %s\n", GetSymName(type)); */
  StackEntry &tos = stack.top();
  Node *newnode = new Node(type, "", location);
  tos.state = newstate;
  stack.push(StackEntry(newdfa, 0, newnode));
}

void Parser::Pop() {
  /* printf("pop\n"); */
  StackEntry tos = stack.top();
  stack.pop();
  Node *newnode = tos.node;

  if (stack.size()) {
    stack.top().node->AddChild(newnode);
  } else {
    rootnode = newnode;
  }
}

bool Parser::AddToken(const TokenInfo &token) {
  int label = Classify(token);
  if (label < 0) {
    return -1;
  }
  /* token.Format(stdout); */

  while (true) {
    StackEntry &tos = stack.top();
    DFA *dfa = tos.dfa;
    DFAState *states = dfa->states;
    DFAState *state = &states[tos.state];
    bool flag = true;

    /* tos.Dump(); */

    for (int i = 0; i < state->n_arcs; ++i) {
      Arc &arc = state->arcs[i];
      auto type = static_cast<Symbol>(grammar->labels[arc.label].type);
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

      else if (IsNonterminal(type)) {
        DFA *itsdfa = grammar->dfas[static_cast<int>(type) - NT_OFFSET];
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
          e.SyntaxError(token.start, "too much input");
          return -1;
        }
      } else {
        e.SyntaxError(token.start, "unexpected", Quote(token.line));
        return -1;
      }
    }
  }
}

Node *Parser::ParseTokens(const TokenBuffer &tokens) {
  for (const auto &token : tokens) {
    if (token.type == Symbol::ERRORTOKEN) {
      e.SyntaxError(token.start, "error token", Quote(token.string));
      return nullptr;
    }
    int ret = AddToken(token);
    if (ret == 1) {
      return rootnode;
    }
    if (ret < 0) {
      return nullptr;
    }
  }
  auto last = tokens.end() - 1;
  e.SyntaxError(last->start, "incomplete input");
  return nullptr;
}

Node *simplecompiler::ParseTokens(const TokenBuffer &tokens) {
  Parser parser(&CompilerGrammar);
  return parser.ParseTokens(tokens);
}
