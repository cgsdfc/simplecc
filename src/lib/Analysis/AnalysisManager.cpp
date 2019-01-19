#include "simplecc/Analysis/AnalysisManager.h"
#include "simplecc/AST/ASTVerifier.h"
#include "simplecc/Analysis/ArrayBoundChecker.h"
#include "simplecc/Analysis/ImplicitCallTransformer.h"
#include "simplecc/Analysis/SymbolTableBuilder.h"
#include "simplecc/Analysis/SyntaxChecker.h"
#include "simplecc/Analysis/TypeChecker.h"

using namespace simplecc;

AnalysisManager::~AnalysisManager() = default;

bool AnalysisManager::runAllAnalyses(ProgramAST *P) {
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

  if (ASTVerifier().Check(P)) {
    PrintErrs("ProgramAST should be well-formed after all analyses run!");
    return true;
  }

  return false;
}