// MIT License
// 
// Copyright (c) 2018 Cong Feng.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef SIMPLECC_PARSE_PARSER_H
#define SIMPLECC_PARSE_PARSER_H
#include "simplecc/Lex/TokenInfo.h"
#include "simplecc/Support/ErrorManager.h"
#include <iostream>
#include <stack>
#include <string>
#include <vector>
#include <memory> // unique_ptr

namespace simplecc {
class Node;

/// @brief Parser is a push-down finite state machine that parses the tokens
/// linearly with one lookahead without any recursion call.
class Parser {

  /// @brief StackEntry represents an entry in the Parser's deduction stack.
  class StackEntry {
    /// @brief TheDFA is the grammar rule that is being expanded.
    const DFA *TheDFA;
    /// @brief TheState is the current state of TheDFA.
    int TheState;
    /// @brief TheNode is the Node being constructed under the grammar rule.
    Node *TheNode;
    // use unique_ptr.

  public:
    StackEntry(const DFA *D, int State, Node *N)
        : TheDFA(D), TheState(State), TheNode(N) {}

    const DFA *getDFA() const { return TheDFA; }
    int getState() const { return TheState; }
    Node *getNode() const { return TheNode; }
    void setState(int S) { TheState = S; }

    /// @brief Debug helper.
    void Format(std::ostream &O) const;
  };

  /// @brief Lookup the label value for a token type and token value.
  /// Return -1 for failure.
  int Classify(Symbol Type, const std::string &Value);

  /// @brief Shift a token, update the state.
  void Shift(const TokenInfo &T, int NewState);

  /// @brief Push a non-terminal onto the stack, update the state.
  void Push(Symbol Ty, const DFA *NewDFA, int NewState, Location Loc);

  /// @brief Pop the stack, add the node of the old TOS as a child to the node of the new TOS.
  void Pop();

  /// @brief Add one token, loop until:
  /// 1. we are done. Return 1.
  /// 2. error. Return -1.
  /// 3. the token was shifted. Return 0.
  int AddToken(const TokenInfo &T);

  /// @brief Return if the label is in the first set of \param D.
  static bool IsInFirstSet(const DFA *D, int Label);

  /// @brief Return if the \param State is an accept-only state.
  /// Accept-only state means this state can only transform to an accept state.
  static bool IsAcceptOnlyState(const DFAState *State);

public:
  /// @brief Construct a Parser from a Grammar object.
  explicit Parser(const Grammar *G);
  // TODO: cleanup the stack.
  ~Parser() = default;

  /// @brief Parse the tokens, return the root of a parse tree if no errors.
  /// Return nullptr on errors.
  std::unique_ptr<Node> ParseTokens(const std::vector<TokenInfo> &Tokens);

private:
  // The parse stack.
  std::stack<StackEntry> TheStack;
  // The grammar rule table.
  const Grammar *TheGrammar;
  // Borrowed pointer to the root of the parse tree.
  Node *RootNode = nullptr;
  // Report syntax error.
  ErrorManager EM;
};

} // namespace simplecc
#endif // SIMPLECC_PARSE_PARSER_H