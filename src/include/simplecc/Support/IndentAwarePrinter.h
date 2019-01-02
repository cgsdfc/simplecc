#ifndef SIMPLECC_SUPPORT_INDENTAWAREPRINTER_H
#define SIMPLECC_SUPPORT_INDENTAWAREPRINTER_H
#include <iostream>

namespace simplecc {

/// This is a CRTP Mixin that provides means to control
/// the indent level in each line to be printed. Subclass
/// should provide a getOS() method for this to work.
template <typename Derive> class IndentAwarePrinter {
public:
  void increaseIndentLevel() { ++IndentLevel; }
  void decreaseIndentLevel() { --IndentLevel; }
  unsigned getIndentLevel() const { return IndentLevel; }
  void printIndent() {
    for (unsigned I = 0, E = getIndentLevel(); I < E; I++) {
      static_cast<Derive *>(this)->getOS() << "  ";
    }
  }

private:
  unsigned IndentLevel = 0;
};

} // namespace simplecc

#endif // SIMPLECC_SUPPORT_INDENTAWAREPRINTER_H