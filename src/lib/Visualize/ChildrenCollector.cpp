#include "simplecc/Visualize/ChildrenCollector.h"
#include "simplecc/Visualize/AstRef.h"

using namespace simplecc;

void ChildrenCollector::Collect(const simplecc::AstRef &R) {
  Children.clear();
  visitAST(R.get());
}