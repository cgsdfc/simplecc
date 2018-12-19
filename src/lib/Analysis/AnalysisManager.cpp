#include "simplecc/Analysis/AnalysisManager.h"

#include "simplecc/Analysis/ImplicitCallTransformer.h"
#include "simplecc/Analysis/AstVerifier.h"
#include "simplecc/Analysis/SyntaxChecker.h"
#include "simplecc/Analysis/TypeChecker.h"
#include "simplecc/Analysis/SymbolTableBuilder.h"

using namespace simplecc;

AnalysisManager::~AnalysisManager() = default;

bool AnalysisManager::runAllAnalyses(Program *P) {
  if (!SyntaxChecker().Check(P)) {
    return false;
  }

  if (!SymbolTableBuilder().Build(P, TheTable)) {
    return false;
  }

  ImplicitCallTransformer().Transform(P, TheTable);

  if (!TypeChecker().Check(P, TheTable)) {
    return false;
  }

  if (!AstVerifier().Verify(P)) {
    PrintErrs("Program should be well-formed after all analyses run!");
    return false;
  }

  return true;
}