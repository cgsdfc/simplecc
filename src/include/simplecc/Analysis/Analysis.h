#ifndef SIMPLECC_ANALYSIS_ANALYSIS_H
#define SIMPLECC_ANALYSIS_ANALYSIS_H
// External Interface to the Analysis Module.
#include "simplecc/Analysis/SymbolTable.h"
#include "simplecc/Analysis/Types.h"

namespace simplecc {
class ProgramAST;

class Analysis {
  SymbolTable TheTable;

public:
  Analysis() = default;
  ~Analysis();

  /// Run **all** the analyses on the Program.
  /// Return true if errors happened.
  bool runAllAnalyses(ProgramAST *P);

  const SymbolTable &getSymbolTable() const { return TheTable; }

  SymbolTable &getSymbolTable() { return TheTable; }

  void clear() { TheTable.clear(); }
};
} // namespace simplecc

#endif // SIMPLECC_ANALYSIS_ANALYSIS_H