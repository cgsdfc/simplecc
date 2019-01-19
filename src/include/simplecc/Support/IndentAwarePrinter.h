#ifndef SIMPLECC_SUPPORT_INDENTAWAREPRINTER_H
#define SIMPLECC_SUPPORT_INDENTAWAREPRINTER_H
#include <iostream>

namespace simplecc {
// use combination:
// class IndentationPrinter {
// public:
//   void indent();
//   void dedent();
//   void printIndent(std::ostream &OS) const;
//   void printNewline(std::ostream &OS) const;
// private:
//   unsigned IndentLevel = 0;
// };
// TODO: fix this mutually dependent strange class.

/// IndentAwarePrinter is a CRTP Mixin that provides means to control
/// the indent level in each line to be printed. Subclass
/// should provide a getOS() method for this to work.
template <typename Derive> class IndentAwarePrinter {
public:
  void increaseIndentLevel() { ++IndentLevel; }
  void decreaseIndentLevel() { --IndentLevel; }
  unsigned getIndentLevel() const { return IndentLevel; }
  void printIndent() {
    for (unsigned I = 0, E = getIndentLevel(); I < E; I++) {
      static_cast<Derive *>(this)->getOstream() << "  ";
    }
  }

private:
  unsigned IndentLevel = 0;
};

} // namespace simplecc
#endif // SIMPLECC_SUPPORT_INDENTAWAREPRINTER_H