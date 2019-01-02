#include "simplecc/IR/IRBuilder.h"
#include "simplecc/IR/BasicBlock.h"
#include "simplecc/IR/Function.h"

using namespace simplecc;

Type *IRBuilder::getCurrentFunctionReturnType() const {
  assert(BB);
  return BB->getParent()->getReturnType();
}
