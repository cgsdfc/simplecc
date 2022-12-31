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

#include "simplecc/Parse/Parser.h"
#include "simplecc/Parse/Node.h"
#include <algorithm> // find_if()
#include <iterator> // next()

using namespace simplecc;

Parser::Parser(const Grammar *G) : TheStack(), TheGrammar(G), EM("SyntaxError") {
  auto Start = G->start;
  Node *Root = new Node(static_cast<Symbol>(Start), "", Location(0, 0));
  TheStack.push(StackEntry(G->dfas[Start - NT_OFFSET], 0, Root));
}

bool Parser::IsAcceptOnlyState(const DFAState *State) {
  return State->is_final && State->n_arcs == 1;
}

bool Parser::IsInFirstSet(const DFA *D, int Label) {
  auto end = D->first + D->n_first;
  return std::find(D->first, end, Label) != end;
}

int Parser::Classify(Symbol Type, const std::string &Value) {
  // the first label item is for the empty label.
  const auto LabelBegin = TheGrammar->labels;
  const auto LabelEnd = TheGrammar->labels + TheGrammar->n_labels;

  // Convert an iterator to an offset(index).
  auto ToIndex = [LabelBegin](decltype(LabelBegin) Iter) {
    return int(Iter - LabelBegin);
  };

  // look for a keyword or operator first.
  if (Type == Symbol::NAME || Type == Symbol::OP) {
    auto Iter = std::find_if(std::next(LabelBegin), LabelEnd,
                             [&Value](const Label &L) { return L.string && L.string == Value; });
    if (Iter != LabelEnd) {
      // the index of the label item is the label value for this token.
      return ToIndex(Iter);
    }
  }

  // look for an ordinary token.
  auto Iter = std::find_if(std::next(LabelBegin), LabelEnd,
                           [Type](const Label &L) { return static_cast<int>(Type) == L.type && !L.string; });
  if (Iter != LabelEnd) {
    return ToIndex(Iter);
  }

  // failure.
  return -1;
}

void Parser::Shift(const TokenInfo &T, int NewState) {
  StackEntry &Top = TheStack.top();
  Top.getNode()->AddChild(
      new Node(T.getType(), T.getString(), T.getLocation()));
  Top.setState(NewState);
}

void Parser::Push(Symbol Ty, const DFA *NewDFA, int NewState, Location Loc) {
  StackEntry &Top = TheStack.top();
  Top.setState(NewState);
  Node *NewNode = new Node(Ty, "", Loc);
  TheStack.push(StackEntry(NewDFA, /* State */ 0, NewNode));
}

void Parser::Pop() {
  StackEntry Top = TheStack.top();
  TheStack.pop();
  Node *NewNode = Top.getNode();
  if (TheStack.empty()) {
    // we are done.
    RootNode = NewNode;
    return;
  }
  TheStack.top().getNode()->AddChild(NewNode);
}

int Parser::AddToken(const TokenInfo &T) {
  // fail fast if it is an error token.
  if (T.getType() == Symbol::ERRORTOKEN) {
    EM.Error(T.getLocation(), "error token", T.getString());
    return -1;
  }

  // classify the token into label value.
  auto Label = Classify(T.getType(), T.getString());
  if (Label < 0) {
    EM.Error(T.getLocation(), "unexpected token", T.getString());
    return -1;
  }

  // loop until we are done.
  while (true) {
    StackEntry &Top = TheStack.top();
    const DFA *D = Top.getDFA();
    DFAState *States = D->states;
    DFAState *TheState = &States[Top.getState()];
    bool Flag = true;

    for (int I = 0; I < TheState->n_arcs; ++I) {
      Arc &arc = TheState->arcs[I];
      auto Ty = static_cast<Symbol>(TheGrammar->labels[arc.label].type);
      int NewState = arc.state;

      if (Label == arc.label) {
        Shift(T, NewState);

        while (IsAcceptOnlyState(&States[NewState])) {
          Pop();
          if (TheStack.empty()) {
            return true;
          }
          NewState = TheStack.top().getState();
          States = TheStack.top().getDFA()->states;
        }
        return false;

      } else if (TokenInfo::IsNonTerminal(Ty)) {
        DFA *NewDFA = TheGrammar->dfas[static_cast<int>(Ty) - NT_OFFSET];
        if (IsInFirstSet(NewDFA, Label)) {
          Push(Ty, NewDFA, NewState, T.getLocation());
          Flag = false;
          break;
        }
      }
    }

    if (Flag) {
      if (TheState->is_final) {
        Pop();
        if (TheStack.empty()) {
          EM.Error(T.getLocation(), "too much input");
          return -1;
        }
      } else {
        EM.Error(T.getLocation(), "unexpected", T.getLine());
        return -1;
      }
    }
  }
}

std::unique_ptr<Node> Parser::ParseTokens(const std::vector<TokenInfo> &Tokens) {
  for (const auto &T : Tokens) {
    auto RC = AddToken(T);
    // error happened.
    if (RC < 0) {
      return nullptr;
    }
    // all tokens parsed successfully.
    if (RC == 1) {
      assert(RootNode && "RootNode cannot be null!");
      // ownership handled to caller.
      return std::unique_ptr<Node>(RootNode);
    }
    // continue...
  }
  // if not return from the loop above, we are screw by extraordinary input.
  const auto &LastToken = Tokens.back();
  EM.Error(LastToken.getLocation(), "incomplete input");
  return nullptr;
}

void Parser::StackEntry::Format(std::ostream &O) const {
  Print(O, "state:", TheState);
  Print(O, "dfa:", TheDFA->name);
}