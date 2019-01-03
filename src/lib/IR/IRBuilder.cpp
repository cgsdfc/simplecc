#include "simplecc/IR/IRBuilder.h"
#include "simplecc/IR/Function.h"

using namespace simplecc;

Type *IRBuilder::getCurrentFunctionReturnType() const {
  assert(BB);
  return BB->getParent()->getReturnType();
}

void IRBuilder::SetInsertPoint(BasicBlock *TheBB, BasicBlock::iterator IP) {
  BB = TheBB;
  InsertPt = IP;
}

void IRBuilder::SetInsertPoint(Instruction *I) {
  BB = I->getParent();
  InsertPt = I->getIterator();
}

void IRBuilder::SetInsertPoint(BasicBlock *TheBB) {
  BB = TheBB;
  InsertPt = BB->end();
}

