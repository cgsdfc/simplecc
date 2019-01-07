#ifndef SIMPLECC_PARSE_PARSER_H
#define SIMPLECC_PARSE_PARSER_H
#include "simplecc/Lex/TokenInfo.h"
#include "simplecc/Support/ErrorManager.h"
#include <iostream>
#include <stack>
#include <vector>

namespace simplecc {
class Node;

class Parser {

  class StackEntry {
    DFA *TheDFA;
    int TheState;
    Node *TheNode;

  public:
    StackEntry(DFA *D, int State, Node *N)
        : TheDFA(D), TheState(State), TheNode(N) {}

    DFA *getDFA() const { return TheDFA; }

    int getState() const { return TheState; }

    Node *getNode() const { return TheNode; }

    void setState(int S) { TheState = S; }

    void Format(std::ostream &O) const;
    friend std::ostream &operator<<(std::ostream &os, const StackEntry &entry) {
      entry.Format(os);
      return os;
    }
  };

  int Classify(const TokenInfo &T);
  void Shift(const TokenInfo &T, int NewState);
  void Push(Symbol Ty, DFA *NewDFA, int NewState, Location Loc);
  void Pop();
  int AddToken(const TokenInfo &T);

  static bool IsInFirst(DFA *D, int Label);
  static bool IsAcceptOnlyState(DFAState *State);

public:
  explicit Parser(const Grammar *G);
  Node *ParseTokens(const std::vector<TokenInfo> &Tokens);

private:
  std::stack<StackEntry> TheStack;
  const Grammar *TheGrammar;
  Node *RootNode = nullptr;
  ErrorManager EM;
};
} // namespace simplecc
#endif