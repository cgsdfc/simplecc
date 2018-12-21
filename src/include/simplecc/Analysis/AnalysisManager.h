#ifndef SIMPLECOMPILER_ANALYSISMANAGER_H
#define SIMPLECOMPILER_ANALYSISMANAGER_H
// External Interface to the Analysis Module.
#include "SymbolTable.h"
#include "Types.h"

namespace simplecc {
class Program;

class AnalysisManager {
  SymbolTable TheTable;
public:
  AnalysisManager() = default;
  ~AnalysisManager();

  /// Run **all** the analyses on the Program.
  /// Return true if errors happened.
  bool runAllAnalyses(Program *P);

  const SymbolTable &getSymbolTable() const {
    return TheTable;
  }

  SymbolTable &getSymbolTable() {
    return TheTable;
  }

  void clear() {
    TheTable.clear();
  }
};
}

#endif //SIMPLECOMPILER_ANALYSISMANAGER_H
