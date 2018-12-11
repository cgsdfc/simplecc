#include "Parser.h"
#include "Grammar.h"
#include "Node.h"
#include "error.h"
#include "TokenInfo.h"

#include <algorithm>
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
    PrintErrs("state:", state);
    PrintErrs("dfa", dfa->name);
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
  Node *ParseTokens(const std::vector<TokenInfo> &tokens);
};

} // namespace

Parser::Parser(Grammar *grammar)
    : stack(), grammar(grammar), rootnode(nullptr), e() {
  auto start = grammar->start;
  Node *newnode = new Node(static_cast<Symbol>(start), "", Location(0, 0));
  stack.push(StackEntry(grammar->dfas[start - NT_OFFSET], 0, newnode));
}

int Parser::Classify(const TokenInfo &token) {
  if (token.getType() == Symbol::NAME || token.getType() == Symbol::OP) {
    for (int i = 1; i < grammar->n_labels; i++) {
      const Label &l = grammar->labels[i];
      if (l.string && l.string == token.getString())
        return i;
    }
  }
  for (int i = 1; i < grammar->n_labels; i++) {
    const Label &l = grammar->labels[i];
    if (l.type == static_cast<int>(token.getType()) && l.string == nullptr) {
      return i;
    }
  }
  e.SyntaxError(token.getLocation(), "unexpected", Quote(token.getString()));
  return -1;
}

void Parser::Shift(const TokenInfo &token, int newstate) {
  StackEntry &tos = stack.top();
  tos.node->AddChild(
      new Node(token.getType(), token.getString(), token.getLocation()));
  tos.state = newstate;
}

void Parser::Push(Symbol type, DFA *newdfa, int newstate,
                  const Location &location) {
  StackEntry &tos = stack.top();
  Node *newnode = new Node(type, "", location);
  tos.state = newstate;
  stack.push(StackEntry(newdfa, 0, newnode));
}

void Parser::Pop() {
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

  while (true) {
    StackEntry &tos = stack.top();
    DFA *dfa = tos.dfa;
    DFAState *states = dfa->states;
    DFAState *state = &states[tos.state];
    bool flag = true;

    for (int i = 0; i < state->n_arcs; ++i) {
      Arc &arc = state->arcs[i];
      auto type = static_cast<Symbol>(grammar->labels[arc.label].type);
      int newstate = arc.state;

      if (label == arc.label) {
        Shift(token, newstate);

        while (IsAcceptOnlyState(&states[newstate])) {
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
          Push(type, itsdfa, newstate, token.getLocation());
          flag = false;
          break;
        }
      }
    }

    if (flag) {
      if (state->is_final) {
        Pop();
        if (stack.empty()) {
          e.SyntaxError(token.getLocation(), "too much input");
          return -1;
        }
      } else {
        e.SyntaxError(token.getLocation(), "unexpected",
                      Quote(token.getLine()));
        return -1;
      }
    }
  }
}

Node *Parser::ParseTokens(const std::vector<TokenInfo> &tokens) {
  for (const auto &token : tokens) {
    if (token.getType() == Symbol::ERRORTOKEN) {
      e.SyntaxError(token.getLocation(), "error token",
                    Quote(token.getString()));
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
  e.SyntaxError(last->getLocation(), "incomplete input");
  return nullptr;
}

Node *simplecompiler::ParseTokens(const std::vector<TokenInfo> &tokens) {
  Parser parser(&CompilerGrammar);
  return parser.ParseTokens(tokens);
}
