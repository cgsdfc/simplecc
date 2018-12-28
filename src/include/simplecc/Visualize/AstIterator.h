#ifndef SIMPLECC_VISUALIZE_ASTITERATOR_H
#define SIMPLECC_VISUALIZE_ASTITERATOR_H
#include "simplecc/Visualize/TreeLikeIterator.h"
#include <vector>

namespace simplecc {
class AstRef;
class AstGraph;
class Program;

/// Iterator to all nodes of a graph.
class AstIterator : public TreeLikeIterator<AstIterator, AstRef *, std::vector<AstRef *>::const_iterator> {
public:
  AstIterator(Program *P, AstGraph *G);
  AstIterator() = default;

  EdgeRange getEdges(value_type N);

private:
  AstGraph *Parent = nullptr;
};
} // namespace simplecc

#endif // SIMPLECC_VISUALIZE_ASTITERATOR_H
