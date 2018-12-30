#include <simplecc/IR/Instructions.h>
#include "simplecc/IR/BasicBlock.h"

using namespace simplecc;

ReturnInstr::ReturnInstr(Value *Val, BasicBlock *BB)
    : Instruction(Type::getVoidType(), Ret, Val ? 1 : 0, BB) {
  if (Val && !Val->getType()->isVoidType()) {
    setOperand(0, Val);
  }
}

BranchInst::BranchInst(unsigned NumOps, BasicBlock *BB)
    : Instruction(Type::getVoidType(), Br, NumOps, BB) {}

BranchInst::BranchInst(BasicBlock *IfTrue,
                       BasicBlock *IfFalse,
                       Value *Cond,
                       BasicBlock *BB)
    : BranchInst(3, BB) {
  setOperand(0, Cond);
  setOperand(1, IfTrue);
  setOperand(2, IfFalse);
}

BranchInst::BranchInst(BasicBlock *IfTrue, BasicBlock *BB) : BranchInst(1, BB) {
  setOperand(0, IfTrue);
}

BasicBlock *BranchInst::getSuccessor(unsigned I) const {
  assert(I < getNumSuccessors() && "Successor # out of range for Branch");
  return static_cast<BasicBlock *>(getOperand(I + isConditional()));
}

void BranchInst::setSuccessor(unsigned I, BasicBlock *NewSucc) {
  assert(I < getNumSuccessors() && "Successor # out of range for Branch");
  setOperand(I + isConditional(), NewSucc);
}

void BranchInst::swapSuccessors() {
  if (isConditional()) {
    auto tmp = getOperand(1);
    setOperand(1, getOperand(2));
    setOperand(2, tmp);
  }
}

LoadInst::LoadInst(Value *Ptr, BasicBlock *IAE)
    : Instruction(Type::getIntType(), Load, 1, IAE) {
  assert(Ptr->getType()->isPointerType() && "ptr required");
  setOperand(0, Ptr);
}

AllocaInst::AllocaInst(unsigned int NumAlloc, BasicBlock *IAE)
    : Instruction(Type::getIntType(), Alloca, 1, IAE), NumAlloc(NumAlloc) {}
