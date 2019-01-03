#include "simplecc/IR/BasicBlock.h"
#include "simplecc/IR/Function.h"
#include "simplecc/IR/Instruction.h"
#include <algorithm>
#include <simplecc/IR/BasicBlock.h>

using namespace simplecc;

void BasicBlock::removeInst(const Instruction *I) {
  auto Iter = std::find(begin(), end(), I);
  if (Iter != end()) {
    InstList.erase(Iter);
  }
}

BasicBlock::BasicBlock(Function *F)
    : Value(Type::getLabelType(), BasicBlockVal), Parent(F) {
  if (Parent) {
    Parent->getBasicBlockList().push_back(this);
  }
}

BasicBlock::~BasicBlock() {
  std::for_each(begin(), end(), ValueDeleter());
}

Instruction *BasicBlock::getTerminator() {
  if (empty())
    return nullptr;
  Instruction *I = &back();
  return I->isTerminator() ? I : nullptr;
}
