#ifndef SIMPLECC_IR_INSTRUCTION_H
#define SIMPLECC_IR_INSTRUCTION_H
#include "simplecc/IR/User.h"
#include "simplecc/IR/BasicBlock.h"

namespace simplecc {
class BasicBlock;
class Module;
class Function;

class Instruction : public User {
  friend class Value;
  BasicBlock *Parent;
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

  /// Get an iterator in parent pointing to this.
  BasicBlock::iterator getIterator() const;
public:
  enum OpcodeKind : unsigned {
#define HANDLE_INSTRUCTION(Class, Opcode, Name) Opcode,
#include "simplecc/IR/Instruction.def"
  };

protected:
  Instruction(Type *Ty, unsigned Opcode, unsigned NumOps,
              BasicBlock *InsertAtEnd);
  ~Instruction();

public:
  Instruction(const Instruction &) = delete;
  Instruction &operator=(const Instruction &) = delete;

  unsigned getOpcode() const { return getValueID() - InstructionVal; }
  const char *getOpcodeName() const { return getOpcodeName(getOpcode()); }
  static const char *getOpcodeName(unsigned Opcode);

  bool isTerminator() const { return isTerminator(getOpcode()); }
  bool isBinaryOp() const { return isBinaryOp(getOpcode()); }
  static bool isTerminator(unsigned Opcode);
  static bool isBinaryOp(unsigned Opcode);

  bool isAssociative() const;
  static bool isAssociative(unsigned Opcode) {
    return Opcode == Add || Opcode == Mul;
  }

  bool isCommutative() const { return isCommutative(getOpcode()); }
  static bool isCommutative(unsigned Opcode) {
    switch (Opcode) {
    case Add:
    case Mul:
      return true;
    default:
      return false;
    }
  }

  bool mayWriteToMemory() const;
  bool mayReadFromMemory() const;
  bool mayReadOrWriteMemory() const {
    return mayReadFromMemory() || mayWriteToMemory();
  }

  bool mayHaveSideEffects() const { return mayWriteToMemory(); }

  bool isSafeToRemove() const;

  Instruction *clone() const;

  bool isIdenticalTo(const Instruction *I) const;
  bool isSameOperationAs(const Instruction *I) const;

  unsigned getNumSuccessors() const;
  BasicBlock *getSuccessor(unsigned Idx) const;
  void setSuccessor(unsigned Idx, BasicBlock *BB);

  static bool InstanceCheck(const Value *V) {
    return V->getValueID() >= Value::InstructionVal;
  }

private:
  /// Default implementation of cloneImpl(), subclass should override this.
  Instruction *cloneImpl() const;
};

} // namespace simplecc

#endif
