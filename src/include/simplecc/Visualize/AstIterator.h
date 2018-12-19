#ifndef SIMPLECOMPILER_ASTITERATOR_H
#define SIMPLECOMPILER_ASTITERATOR_H
#include <llvm/ADT/iterator.h>
#include <vector>

namespace simplecc {
class AstRef;
class Program;
class AstGraph;

/// Implementation of AstIterator
class AstIteratorImpl {
  std::vector<AstRef *> TheStack;
  AstGraph *Parent;

public:
  /// Begin Iterator.
  AstIteratorImpl(Program *Ptr, AstGraph *G);

  /// End Iterator.
  AstIteratorImpl() : TheStack() {}
  /// Return the next node in the graph.
  AstRef *getNext();
};

/// Iterator to all nodes of a graph.
class AstIterator
    : private AstIteratorImpl,
      public llvm::iterator_facade_base<AstIterator, std::forward_iterator_tag,
                                        AstRef *> {

public:
  AstIterator(Program *P, AstGraph *G) : AstIteratorImpl(P, G) { operator++(); }

  AstIterator() : AstIteratorImpl(), Ref() {}

  AstRef *operator*() const { return Ref; }
  bool operator==(const AstIterator &O) const { return O.Ref == Ref; }

  AstIterator &operator++() {
    Ref = getNext();
    return *this;
  }

private:
  AstRef *Ref;
};
}

#endif //SIMPLECOMPILER_ASTITERATOR_H
