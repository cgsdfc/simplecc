#ifndef SIMPLECC_OPTIMIZE_INSTRUCTION_H
#define SIMPLECC_OPTIMIZE_INSTRUCTION_H
#include "simplecc/IR/User.h"

namespace simplecc {
class BasicBlock;
class Module;
class Function;

class Instruction : public User {
  BasicBlock *Parent;
protected:
  ~Instruction();

public:
  const BasicBlock *getParent() const { return Parent; }
  BasicBlock *getParent() { return Parent; }

  const Module *getModule() const;
  Module *getModule() {
    return const_cast<Module *>(
        const_cast<const Instruction *>(this)->getModule());
  }

  const Function *getFunction() const;
  Function *getFunction() {
    return const_cast<Function *>(
        const_cast<const Instruction *>(this)->getFunction());
  }

  void removeFromParent();
  void insertBefore(Instruction *InsertPos);
  void insertAfter(Instruction *InsertPos);
  void moveBefore(Instruction *MovePos);
  void moveAfter(Instruction *MovePos);

public:
  enum OpcodeKind : unsigned {
    // Terminator Instructions
        Ret = 1,
    Br,
    TermOpsEnd,

    // UnaryOperator:
        Neg,
    UnaryOpsEnd,

    // BinaryOperator:
        Add,
    Sub,
    Mul,
    Div,
    Shl,
    LShr,
    And,
    Or,
    Xor,
    BinaryOpsEnd,

    // Memory Instructions
        Alloca,
    Load,
    Store,
    GetElementPtr,
    MemoryOpsEnd,

    // Other Instructions
        ICmp,
    PHI,
    Call,
    OtherOpsEnd,

    // Begin values.
        TermOpsBegin = Ret,
    UnaryOpsBegin = Neg,
    BinaryOpsBegin = Add,
    MemoryOpsBegin = Alloca,
    OtherOpsBegin = ICmp,
  };

protected:
  Instruction(Type *Ty, unsigned iType, unsigned NumOps, Instruction *InsertBefore = nullptr);
  Instruction(Type *Ty, unsigned iType, unsigned NumOps, BasicBlock *InsertAtEnd = nullptr);

public:
  Instruction(const Instruction &) = delete;
  Instruction &operator=(const Instruction &) = delete;

  unsigned getOpcode() const { return getValueID() - InstructionVal; }
  const char *getOpcodeName() const { return getOpcodeName(getOpcode()); }

  bool isTerminator() const { return isTerminator(getOpcode()); }
  bool isUnaryOp() const { return isUnaryOp(getOpcode()); }
  bool isBinaryOp() const { return isBinaryOp(getOpcode()); }

  /// subclass classification
  static const char *getOpcodeName(unsigned Opcode);

  static bool isTerminator(unsigned Opcode) {
    return TermOpsBegin <= Opcode && Opcode < TermOpsEnd;
  }

  static bool isUnaryOp(unsigned Opcode) {
    return UnaryOpsBegin <= Opcode && Opcode < UnaryOpsEnd;
  }

  static bool isBinaryOp(unsigned Opcode) {
    return BinaryOpsBegin <= Opcode && Opcode < BinaryOpsEnd;
  }

  bool isAssociative() const;
  static bool isAssociative(unsigned Opcode) {
    return Opcode == And || Opcode == Or || Opcode == Xor ||
        Opcode == Add || Opcode == Mul;
  }

  bool isCommutative() const { return isCommutative(getOpcode()); }
  static bool isCommutative(unsigned Opcode) {
    switch (Opcode) {
    case Add:
    case Mul:
    case And:
    case Or:
    case Xor:return true;
    default:return false;
    }
  }

  bool mayWriteToMemory() const;
  bool mayReadFromMemory() const;
  bool mayReadOrWriteMemory() const {
    return mayReadFromMemory() || mayWriteToMemory();
  }

  bool mayHaveSideEffects() const {
    return mayWriteToMemory();
  }

  bool isSafeToRemove() const;

  Instruction *clone() const;

  bool isIdenticalTo(const Instruction *I) const;
  bool isSameOperationAs(const Instruction *I) const;

  unsigned getNumSuccessors() const;
  BasicBlock *getSuccessor(unsigned Idx) const;
  void setSuccessor(unsigned Idx, BasicBlock *BB);

  static bool InstanceCheck(const Value *V) {
    // XXX: Any valueID == InstructionVal??
    return V->getValueID() >= Value::InstructionVal;
  }

private:
  /// Default implementation of cloneImpl(), subclass should override this.
  Instruction *cloneImpl() const;
};

} // namespace simplecc

#endif
