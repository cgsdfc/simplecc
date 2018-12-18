#ifndef PARSER_H
#define PARSER_H
#include "simplecc/ErrorManager.h"
#include "simplecc/TokenInfo.h"

#include <stack>
#include <vector>

namespace simplecc {
class Node;

class Parser {
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

  int Classify(const TokenInfo &T);
  void Shift(const TokenInfo &T, int NewState);
  void Push(Symbol Ty, DFA *NewDFA, int NewState, const Location &Loc);
  void Pop();
  int AddToken(const TokenInfo &T);

  static bool IsInFirst(DFA *D, int Label);
  static bool IsAcceptOnlyState(DFAState *State);

public:
  explicit Parser(Grammar *G);
  Node *ParseTokens(const std::vector<TokenInfo> &Tokens);

private:
  std::stack<StackEntry> TheStack;
  Grammar *TheGrammar;
  Node *RootNode = nullptr;
  ErrorManager EM;
};

Node *ParseTokens(const std::vector<TokenInfo> &Tokens);
} // namespace simplecc
#endif
