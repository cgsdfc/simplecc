#ifndef SIMPLECC_INSTRUCTIONS_H
#define SIMPLECC_INSTRUCTIONS_H
#include "simplecc/IR/Instruction.h"
#include <cassert>

/// All subclasses of Instruction defined here.
namespace simplecc {

/// This class represents the ret instruction. ret is a terminator instruction
/// that terminates the execution of a function. Its type is void. It optionally
/// accepts a Value returned to the caller. If the return Value is void, it is
/// treated as no return Value. Otherwise the type of it must be int. ret int %1
/// ret void
class ReturnInstr : public Instruction {
private:
  explicit ReturnInstr(Value *Val, BasicBlock *BB);
  friend class Instruction;

public:
  /// Return the ret value. If it has none, return nullptr.
  Value *getReturnValue() const {
    return getNumOperands() != 0 ? getOperand(0) : nullptr;
  }

  unsigned getNumSuccessors() const { return 0; }

  static ReturnInstr *Create(Value *Val, BasicBlock *BB) {
    return new ReturnInstr(Val, BB);
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

/// This class represents the br instruction.
/// The br instruction represents either a conditional branch or unconditional
/// branch. When it is a conditional branch, it accepts 3 Values, one for the
/// condition, one for the BB when the condition is true and one for the BB when
/// the condition is false. When it is an unconditional branch, it only accepts
/// the BB to branch to. It returns no Value. Its type is void. br label %end br
/// int %cmp, label %true, label %false.
class BranchInst : public Instruction {
private:
  BranchInst(unsigned NumOps, BasicBlock *BB);
  BranchInst(BasicBlock *IfTrue, BasicBlock *BB);
  BranchInst(BasicBlock *IfTrue, BasicBlock *IfFalse, Value *Cond,
             BasicBlock *BB);

public:
  static BranchInst *Create(BasicBlock *IfTrue, BasicBlock *IAE = nullptr) {
    return new BranchInst(IfTrue, IAE);
  }
  static BranchInst *Create(BasicBlock *IfTrue, BasicBlock *IfFalse,
                            Value *Cond, BasicBlock *IAE) {
    return new BranchInst(IfTrue, IfFalse, Cond, IAE);
  }
  static bool InstanceCheck(const Instruction *I) {
    return I->getOpcode() == Instruction::Br;
  }

  bool isUnconditional() const { return getNumOperands() == 1; }
  bool isConditional() const { return getNumOperands() == 3; }

  Value *getCondition() const {
    assert(isConditional() &&
           "Cannot get condition of an unconditional branch!");
    return getOperand(0);
  }

  void setCondition(Value *V) {
    assert(isConditional() &&
           "Cannot set condition of an unconditional branch!");
    setOperand(0, V);
  }

  unsigned getNumSuccessors() const { return 1 + isConditional(); }
  BasicBlock *getSuccessor(unsigned I) const;
  void setSuccessor(unsigned I, BasicBlock *NewSucc);
  void swapSuccessors();
};

/// This class represents the load instruction.
/// The load instruction accepts a ptr and returns an int as the loaded value.
/// %1 = load ptr %2
class LoadInst : public Instruction {
  LoadInst(Value *Ptr, BasicBlock *IAE);

public:
  LoadInst *Create(Value *Ptr, BasicBlock *IAE) {
    return new LoadInst(Ptr, IAE);
  }
  /// Return the ptr being loaded.
  Value *getPtr() const { return getOperand(0); }
  static bool InstanceCheck(const Instruction *I) {
    return I->getOpcode() == Instruction::Load;
  }
};

/// This class represents the alloca instruction.
/// AllocaInst allocates N Int on the stack and return a ptr to the first
/// element. The type of it is PointerType.
class AllocaInst : public Instruction {
  explicit AllocaInst(unsigned int NumAlloc, BasicBlock *IAE);

public:
  static AllocaInst *Create(unsigned int NumAlloc, BasicBlock *IAE) {
    return new AllocaInst(NumAlloc, IAE);
  }

  /// Return the numeber of int allocated.
  unsigned int getNumAlloc() const { return NumAlloc; }
  static bool InstanceCheck(const Instruction *I) {
    return I->getOpcode() == Alloca;
  }

private:
  unsigned NumAlloc;
};

/// GetElementPtrInst accepts a base ptr and an offset and returns a ptr to the
/// element at the specific offset of the base ptr. The type of it is PointerTy.
class GetElementPtrInst : public Instruction {
  GetElementPtrInst(Value *BasePtr, Value *Offset, BasicBlock *IAE);

public:
  static GetElementPtrInst *Create(Value *BasePtr, Value *Offset,
                                   BasicBlock *IAE) {
    new GetElementPtrInst(BasePtr, Offset, IAE);
  }
  Value *getBasePtr() const { return getOperand(0); }
  Value *getOffset() const { return getOperand(1); }
  static bool InstanceCheck(const Instruction *I) {
    return I->getOpcode() == GEP;
  }
};

/// StoreInst accepts a ptr and an int and store that int into the address. It
/// does not return a Value so its type is VoidTy.
class StoreInst : public Instruction {
  StoreInst(Value *Ptr, Value *Val, BasicBlock *IAE);

public:
  static StoreInst *Create(Value *Ptr, Value *Val, BasicBlock *IAE) {
    return new StoreInst(Ptr, Val, IAE);
  }
  Value *getPtr() const { return getOperand(0); }
  Value *getValue() const { return getOperand(1); }
  static bool InstanceCheck(const Instruction *I) {
    return I->getOpcode() == Store;
  }
};

/// BinaryOperator performs 4 basic arithmetic on its operands. Its name depends
/// on its opcode, i.e., add, sub, mul and div. All these operations are signed.
class BinaryOperator : public Instruction {
  BinaryOperator(unsigned Op, Value *LHS, Value *RHS, BasicBlock *IAE);

public:
  static BinaryOperator *Create(unsigned Op, Value *LHS, Value *RHS,
                                BasicBlock *IAE) {
    return new BinaryOperator(Op, LHS, RHS, IAE);
  }
/// Creator of all kinds of BinaryOperators.
#define HANDLE_BINARY_OPERATOR(Class, Opcode, Name)                            \
  static BinaryOperator *Create##Opcode(Value *LHS, Value *RHS,                \
                                        BasicBlock *IAE) {                     \
    return Create((Opcode), LHS, RHS, IAE);                                    \
  }
#include "simplecc/IR/Instruction.def"
  Value *getLeft() const { return getOperand(0); }
  Value *getRight() const { return getOperand(1); }
  static bool InstanceCheck(const Instruction *I) { return I->isBinaryOp(); }
};

/// PHINode is the multiplexer that accept a list of pair of an incoming BB and
/// a Value, and select one of the Value if the control flow comes from the
/// corresponding BB.
class PHINode : public Instruction {
  /// Construct an empty PHINode -- has no BB or Value.
  explicit PHINode(BasicBlock *IAE);

public:
  static PHINode *Create(BasicBlock *IAE) { return new PHINode(IAE); }
  static bool InstanceCheck(const Instruction *I) {
    return I->getOpcode() == PHI;
  }
  void addIncoming(Value *V, BasicBlock *BB);
};

/// The CallInst applies a list of Values to a Function and return its result.
/// The type of it is the return type of the callee.
class CallInst : public Instruction {
  /// The layout is: arg1, arg2, ..., callee.
  CallInst(Function *Callee, const std::vector<Value *> &Args, BasicBlock *IAE);

public:
  static CallInst *Create(Function *Callee, const std::vector<Value *> &Args,
                          BasicBlock *IAE) {
    return new CallInst(Callee, Args, IAE);
  }

  using arg_iterator = User::op_iterator;
  using const_arg_iterator = User::const_op_iterator;

  arg_iterator arg_begin() { return op_begin(); }
  const_arg_iterator arg_begin() const { return op_begin(); }
  arg_iterator arg_end() { return op_end() - 1; }
  const_arg_iterator arg_end() const { return op_end(); }

  bool arg_empty() const { return arg_end() == arg_begin(); }
  size_t arg_size() const { return arg_end() - arg_begin(); }

  iterator_range<arg_iterator> args() {
    return make_range(arg_begin(), arg_end());
  }
  iterator_range<const_arg_iterator> args() const {
    return make_range(arg_begin(), arg_end());
  }
  Value *getCalleeOperand() const { return getOperand(getNumOperands() - 1); }
  static bool InstanceCheck(const Instruction *I) {
    return I->getOpcode() == Call;
  }
};

/// CmpInst perform rich comparison on its 2 operands. Both operands must be int
/// and the result type is also int.
class ICmpInst : public Instruction {
  ICmpInst(unsigned Predicate, Value *LHS, Value *RHS, BasicBlock *IAE);
  static ICmpInst *Create(unsigned Predicate, Value *LHS, Value *RHS,
                          BasicBlock *IAE) {
    return new ICmpInst(Predicate, LHS, RHS, IAE);
  }

public:
  enum PredicateKind {
#define HANDLE_ICMP_PREDICATE(Class, Opcode, Name) Opcode,
#include "simplecc/IR/Instruction.def"
  };

#define HANDLE_ICMP_PREDICATE(Class, Opcode, Name)                             \
  static ICmpInst *Create##Opcode(Value *LHS, Value *RHS, BasicBlock *IAE) {   \
    return new ICmpInst(Opcode, LHS, RHS, IAE);                                \
  }
#include "simplecc/IR/Instruction.def"

  unsigned getPredicate() const { return Predicate; }
  static bool InstanceCheck(const Instruction *I) {
    return I->getOpcode() == ICmp;
  }

private:
  unsigned Predicate;
};

} // namespace simplecc
#endif // SIMPLECC_INSTRUCTIONS_H