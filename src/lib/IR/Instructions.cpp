#include "simplecc/IR/BasicBlock.h"
#include "simplecc/IR/Function.h"
#include <simplecc/IR/Instructions.h>

using namespace simplecc;

ReturnInst::ReturnInst(Value *Val, BasicBlock *BB)
    : Instruction(Type::getVoidType(), Ret, Val ? 1 : 0, BB) {
  if (Val && !Val->getType()->isVoidType()) {
    setOperand(0, Val);
  }
}

BranchInst::BranchInst(unsigned NumOps, BasicBlock *BB)
    : Instruction(Type::getVoidType(), Br, NumOps, BB) {}

BranchInst::BranchInst(BasicBlock *IfTrue, BasicBlock *IfFalse, Value *Cond,
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
    : Instruction(Type::getPointerType(), Alloca, 1, IAE), NumAlloc(NumAlloc) {}

GetElementPtrInst::GetElementPtrInst(Value *BasePtr, Value *Offset,
                                     BasicBlock *IAE)
    : Instruction(Type::getPointerType(), GEP, 2, IAE) {
  assert(BasePtr->getType()->isPointerType());
  assert(Offset->getType()->isIntType());
  setOperand(0, BasePtr);
  setOperand(1, Offset);
}

StoreInst::StoreInst(Value *Ptr, Value *Val, BasicBlock *IAE)
    : Instruction(Type::getVoidType(), Store, 2, IAE) {
  assert(Ptr->getType()->isPointerType());
  assert(Val->getType()->isIntType());
  setOperand(0, Ptr);
  setOperand(1, Val);
}

BinaryOperator::BinaryOperator(unsigned Op, Value *LHS, Value *RHS,
                               BasicBlock *IAE)
    : Instruction(Type::getIntType(), Op, 2, IAE) {
  assert(isBinaryOp(Op) && "Not a BinaryOp");
  assert(LHS->getType()->isIntType() && RHS->getType()->isIntType());
  setOperand(0, LHS);
  setOperand(1, RHS);
}

CallInst::CallInst(Function *Callee, const std::vector<Value *> &Args,
                   BasicBlock *IAE)
    : Instruction(Callee->getReturnType(), Call, Args.size() + 1, IAE) {
  unsigned I = 0;
  for (unsigned E = Args.size(); I < E; ++I) {
    setOperand(I, Args[I]);
  }
  setOperand(I, Callee);
}

ICmpInst::ICmpInst(unsigned Predicate, Value *LHS, Value *RHS, BasicBlock *IAE)
    : Instruction(Type::getIntType(), ICmp, 2, IAE) {
  setOperand(0, LHS);
  setOperand(1, RHS);
}

PHINode::PHINode(BasicBlock *IAE)
    : Instruction(Type::getIntType(), PHI, 0, IAE) {}