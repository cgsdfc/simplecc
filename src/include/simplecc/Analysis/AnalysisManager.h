/// @file External interface to the Analysis module.
#ifndef SIMPLECC_ANALYSIS_ANALYSIS_H
#define SIMPLECC_ANALYSIS_ANALYSIS_H
#include "simplecc/Analysis/SymbolTable.h"
#include "simplecc/Analysis/Types.h"

// TODO: this class sounds very silly.

namespace simplecc {
/// @brief AnalysisManager hides the details of all analyses and provides a simple
/// interface to run all analyses on a program.
class AnalysisManager {
  SymbolTable TheTable;

public:
  AnalysisManager() = default;
  ~AnalysisManager();

  /// Run **all** the analyses on the Program.
  /// Return true if errors happened.
  bool runAllAnalyses(ProgramAST *P);

  /// Return the symbol table backing the analyses.
  const SymbolTable &getSymbolTable() const { return TheTable; }

  SymbolTable &getSymbolTable() { return TheTable; }

  void clear() { TheTable.clear(); }
};
} // namespace simplecc

#endif // SIMPLECC_ANALYSIS_ANALYSIS_H