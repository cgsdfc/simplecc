#include "simplecc/Parser.h"
#include "simplecc/Node.h"

#include <algorithm>

#define ERROR_TYPE "SyntaxError"

using namespace simplecc;

Parser::Parser(Grammar *G)
    : TheStack(), TheGrammar(G), EM(ERROR_TYPE) {
  auto Start = G->start;
  Node *Root = new Node(static_cast<Symbol>(Start), "", Location(0, 0));
  TheStack.push(StackEntry(G->dfas[Start - NT_OFFSET], 0, Root));
}

bool Parser::IsAcceptOnlyState(DFAState *State) {
  return State->is_final && State->n_arcs == 1;
}

bool Parser::IsInFirst(DFA *D, int Label) {
  auto end = D->first + D->n_first;
  return std::find(D->first, end, Label) != end;
}

int Parser::Classify(const TokenInfo &T) {
  if (T.getType() == Symbol::NAME || T.getType() == Symbol::OP) {
    for (int I = 1; I < TheGrammar->n_labels; I++) {
      const Label &L = TheGrammar->labels[I];
      if (L.string && L.string == T.getString())
        return I;
    }
  }
  for (int I = 1; I < TheGrammar->n_labels; I++) {
    const Label &L = TheGrammar->labels[I];
    if (L.type == static_cast<int>(T.getType()) && L.string == nullptr) {
      return I;
    }
  }
  EM.Error(T.getLocation(), "unexpected", Quote(T.getString()));
  return -1;
}

void Parser::Shift(const TokenInfo &T, int NewState) {
  StackEntry &Top = TheStack.top();
  Top.getNode()->AddChild(
      new Node(T.getType(), T.getString(), T.getLocation()));
  Top.setState(NewState);
}

void Parser::Push(Symbol Ty, DFA *NewDFA, int NewState,
                  const Location &Loc) {
  StackEntry &Top = TheStack.top();
  Top.setState(NewState);
  Node *NewNode = new Node(Ty, "", Loc);
  TheStack.push(StackEntry(NewDFA, /* State */ 0, NewNode));
}

void Parser::Pop() {
  StackEntry Top = TheStack.top();
  TheStack.pop();
  Node *NewNode = Top.getNode();

  if (TheStack.size()) {
    TheStack.top().getNode()->AddChild(NewNode);
  } else {
    RootNode = NewNode;
  }
}

int Parser::AddToken(const TokenInfo &T) {
  int Label = Classify(T);
  if (Label < 0) {
    return -1;
  }

  while (true) {
    StackEntry &Top = TheStack.top();
    DFA *D = Top.getDFA();
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

      } else if (IsNonterminal(Ty)) {
        DFA *NewDFA = TheGrammar->dfas[static_cast<int>(Ty) - NT_OFFSET];
        if (IsInFirst(NewDFA, Label)) {
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
        EM.Error(T.getLocation(), "unexpected",
                       Quote(T.getLine()));
        return -1;
      }
    }
  }
}

Node *Parser::ParseTokens(const std::vector<TokenInfo> &Tokens) {
  for (const auto &T : Tokens) {
    if (T.getType() == Symbol::ERRORTOKEN) {
      EM.Error(T.getLocation(), "error token",
                     Quote(T.getString()));
      return nullptr;
    }
    int RC = AddToken(T);
    if (RC == 1) {
      return RootNode;
    }
    if (RC < 0) {
      return nullptr;
    }
  }
  auto LastToken = Tokens.end() - 1;
  EM.Error(LastToken->getLocation(), "incomplete input");
  return nullptr;
}

Node *simplecc::ParseTokens(const std::vector<TokenInfo> &Tokens) {
  Parser P(&CompilerGrammar);
  return P.ParseTokens(Tokens);
}

void Parser::StackEntry::Format(std::ostream &O) const {
  Print(O, "state:", TheState);
  Print(O, "dfa:", TheDFA->name);
}