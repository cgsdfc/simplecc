#ifndef SIMPLECC_PARSE_PARSER_H
#define SIMPLECC_PARSE_PARSER_H
#include "simplecc/Lex/TokenInfo.h"
#include "simplecc/Support/ErrorManager.h"
#include "simplecc/Support/Macros.h"
#include <iostream>
#include <stack>
#include <vector>

namespace simplecc {
class Node;

/// @brief Parser is a push-down finite state machine that parses the tokens
/// linearly with one lookahead without any recursion call.
class Parser {

  /// @brief StackEntry represents an entry in the Parser's deduction stack.
  class StackEntry {
    /// @brief TheDFA is the grammar rule that is being expanded.
    DFA *TheDFA;
    /// @brief TheState is the current state of TheDFA.
    int TheState;
    /// @brief TheNode is the Node being constructed under the grammar rule.
    Node *TheNode;

  public:
    StackEntry(DFA *D, int State, Node *N)
        : TheDFA(D), TheState(State), TheNode(N) {}

    DFA *getDFA() const { return TheDFA; }
    int getState() const { return TheState; }
    Node *getNode() const { return TheNode; }
    void setState(int S) { TheState = S; }

    void Format(std::ostream &O) const;
  };

  /// @brief
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
#endif // SIMPLECC_PARSE_PARSER_H