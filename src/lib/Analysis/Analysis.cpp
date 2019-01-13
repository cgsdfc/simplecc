#include "simplecc/Analysis/Analysis.h"
#include "simplecc/Analysis/ASTVerifier.h"
#include "simplecc/Analysis/ArrayBoundChecker.h"
#include "simplecc/Analysis/ImplicitCallTransformer.h"
#include "simplecc/Analysis/SymbolTableBuilder.h"
#include "simplecc/Analysis/SyntaxChecker.h"
#include "simplecc/Analysis/TypeChecker.h"

using namespace simplecc;

Analysis::~Analysis() = default;

bool Analysis::runAllAnalyses(ProgramAST *P) {
  if (SyntaxChecker().Check(P)) {
    return true;
  }

  if (SymbolTableBuilder().Build(P, TheTable)) {
    return true;
  }

  ImplicitCallTransformer().Transform(P, TheTable);

  if (TypeChecker().Check(P, TheTable)) {
    return true;
  }

  if (ArrayBoundChecker().Check(P, TheTable)) {
    return true;
  }

  if (ASTVerifier().Verify(P)) {
    PrintErrs("ProgramAST should be well-formed after all analyses run!");
    return true;
  }

  return false;
}