#include "simplecc/Optimize/BasicBlock.h"
#include <algorithm>

using namespace simplecc;

void BasicBlock::removeInst(const Instruction *I) {
  auto Iter = std::find(begin(), end(), I);
  if (Iter != end()) {
    InstList.erase(Iter);
  }
}
