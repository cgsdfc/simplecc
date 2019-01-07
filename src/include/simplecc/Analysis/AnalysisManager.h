#ifndef SIMPLECC_ANALYSIS_ANALYSISMANAGER_H
#define SIMPLECC_ANALYSIS_ANALYSISMANAGER_H
// External Interface to the Analysis Module.
#include "SymbolTable.h"
#include "Types.h"

namespace simplecc {
class ProgramAST;

class AnalysisManager {
  SymbolTable TheTable;

public:
  AnalysisManager() = default;
  ~AnalysisManager();

  /// Run **all** the analyses on the Program.
  /// Return true if errors happened.
  bool runAllAnalyses(ProgramAST *P);

  const SymbolTable &getSymbolTable() const { return TheTable; }

  SymbolTable &getSymbolTable() { return TheTable; }

  void clear() { TheTable.clear(); }
};
} // namespace simplecc

#endif // SIMPLECC_ANALYSIS_ANALYSISMANAGER_H
