#ifndef SIMPLECC_INSTRUCTIONS_H
#define SIMPLECC_INSTRUCTIONS_H
#include "simplecc/IR/Instruction.h"
#include <cassert>

/// All subclasses of Instruction defined here.
namespace simplecc {
/// ret void
/// ret Val
class ReturnInstr : public Instruction {
private:
  ReturnInstr(Value *Val = nullptr);
  friend class Instruction;

public:
  Value *getReturnValue() const {
    return getNumOperands() != 0 ? getOperand(0) : nullptr;
  }

  unsigned getNumSuccessors() const { return 0; }

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
  static BranchInst *Create(BasicBlock *IfTrue, BasicBlock *IfFalse, Value *Cond) {
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

class LoadInst : public Instruction {

};

/// AllocaInst allocates N Int on the stack and return a ptr to the first element.
/// The type of it is PointerTy.
class AllocaInst : public Instruction {
  AllocaInst(Value *NumAlloc);
public:
  static AllocaInst *Create(Value *NumAlloc);
  Value *getNumAlloc() const;
  static bool InstanceCheck(const Instruction *I) {
    return I->getOpcode() == Alloca;
  }
};

/// GetElementPtrInst accepts a base ptr and an offset and returns a ptr to the element at the
/// specific offset of the base ptr. The type of it is PointerTy.
class GetElementPtrInst : public Instruction {
  GetElementPtrInst(Value *BasePtr, Value *Offset);
public:
  static GetElementPtrInst *Create(Value *BasePtr, Value *Offset);
  Value *getBasePtr() const;
  Value *getOffset() const;
  static bool InstanceCheck(const Instruction *I) {
    return I->getOpcode() == GetElementPtr;
  }
};

/// StoreInst accepts a ptr and an int and store that int into the address. It does not return a Value
/// so its type is VoidTy.
class StoreInst : public Instruction {
  StoreInst(Value *Ptr, Value *Val);
public:
  static StoreInst *Create(Value *Ptr, Value *Val);
  Value *getStoreToPtr() const;
  Value *getStoredValue() const;

  static bool InstanceCheck(const Instruction *I) {
    return I->getOpcode() == Store;
  }
};

/// BinaryOperator performs 4 basic arithmetic on its operands. Its name depends on its
/// opcode, i.e., add, sub, mul and div. All these operations are signed.
class BinaryOperator : public Instruction {
  BinaryOperator(unsigned Op, Value *LHS, Value *RHS);
public:
  static BinaryOperator *Create(unsigned Op, Value *LHS, Value *RHS);
  static bool InstanceCheck(const Instruction *I) {
    return I->isBinaryOp();
  }
};

/// PHINode is the multiplexer that accept a list of pair of an incoming BB and a Value,
/// and select one of the Value if the control flow comes from the corresponding BB.
class PHINode : public Instruction {
  /// Construct an empty PHINode -- has no BB or Value.
  PHINode();
public:
  void addIncoming(Value *V, BasicBlock *BB);
};

/// The CallInst applies a list of Values to a Function and return its result.
/// The type of it is the return type of the callee.
class CallInst : public Instruction {
  /// The layout is: arg1, arg2, ..., callee.
  CallInst(Function *Callee, const std::vector<Value *> Args);
public:
  User::op_iterator arg_begin() { return op_begin(); }
  User::const_op_iterator arg_begin() const { return op_begin(); }
  User::op_iterator arg_end() { return op_end() - 1; }
  User::const_op_iterator arg_end() const { return op_end(); }

  bool arg_empty() const { return arg_end() == arg_begin(); }
  size_t arg_size() const { return arg_end() - arg_begin(); }

  iterator_range<User::op_iterator> args() {
    return make_range(arg_begin(), arg_end());
  }
  iterator_range<User::const_op_iterator> args() const {
    return make_range(arg_begin(), arg_end());
  }
  Value *getCalleeOperand() const { return getOperand(getNumOperands() - 1); }
  static bool InstanceCheck(const Instruction *I) {
    return I->getOpcode() == Call;
  }
};

/// CmpInst perform rich comparison on its 2 operands. Both operands must be int
/// and the result type is also int.
class CmpInst : public Instruction {
  CmpInst(unsigned Predicate, Value *LHS, Value *RHS);
public:
  static CmpInst *Create(unsigned Predicate, Value *LHS, Value *RHS);
  unsigned getPredicate() const;
  static bool InstanceCheck(const Instruction *I);
};

}
#endif //SIMPLECC_INSTRUCTIONS_H