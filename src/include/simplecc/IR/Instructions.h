#ifndef SIMPLECC_INSTRUCTIONS_H
#define SIMPLECC_INSTRUCTIONS_H
#include <cassert>
#include "simplecc/IR/Instruction.h"

namespace simplecc {
/// ret void
/// ret Val
class ReturnInstr : public Instruction {
private:
  ReturnInstr(Value *Val = nullptr);
protected:
  friend class Instruction;

public:
  Value *getReturnValue() const {
    return getNumOperands() != 0 ? getOperand(0) : nullptr;
  }

  unsigned getNumSuccessors() const { return 0;}

  static ReturnInstr *Create(Value *Val = nullptr) {
    return new ReturnInstr(Val);
  }
  static bool InstanceCheck(const Instruction *I) {
    return I->getOpcode() == Instruction::Ret;
  }
private:
  BasicBlock *getSuccessor(unsigned idx) const {
    assert(false && "ReturnInst has no successors!");
  }
  void setSuccessor(unsigned idx, BasicBlock *B) {
    assert(false && "ReturnInst has no successors!");
  }
};

/// br B
/// br C T F
class BranchInst : public Instruction {
private:
  explicit BranchInst(BasicBlock *IfTrue);
  BranchInst(BasicBlock *IfTrue, BasicBlock *IfFalse, Value *Cond);
public:
  static BranchInst *Create(BasicBlock *IfTrue) {
    return new BranchInst(IfTrue);
  }
  static BranchInst *Create(BasicBlock *IfTrue,  BasicBlock *IfFalse, Value *Cond) {
    return new BranchInst(IfTrue, IfFalse, Cond);
  }
  static bool InstanceCheck(const Instruction *I) {
    return I->getOpcode() == Instruction::Br;
  }

  bool isUnconditional() const { return getNumOperands() == 1; }
  bool isConditional() const { return getNumOperands() == 3; }

  Value *getCondition() const {
    assert(isConditional() && "Cannot get condition of an unconditional branch!");
    return getOperand(0);
  }

  void setCondition(Value *V) {
    assert(isConditional() && "Cannot set condition of an unconditional branch!");
    setOperand(0, V);
  }

  unsigned getNumSuccessors() const { return 1 + isConditional(); }
  BasicBlock *getSuccessor(unsigned I) const {
    assert(I < getNumSuccessors() && "Successor # out of range for Branch");
    return nullptr;
  }
  void setSuccessor(unsigned I, BasicBlock *NewSucc) {
    assert(I < getNumSuccessors() && "Successor # out of range for Branch");
  }
  void swapSuccessors();
};

class UnaryInstruction : public Instruction {

};

class LoadInst : public UnaryInstruction {

};

class AllocaInst : public UnaryInstruction {

};

class GetElementPtrInst : public Instruction {

};

class StoreInst : public Instruction {

};

class BinaryOperator : public Instruction {

};

class PHINode : public Instruction {

};

class CallInst : public Instruction {

};

class CmpInst : public Instruction {

};

}
#endif //SIMPLECC_INSTRUCTIONS_H
