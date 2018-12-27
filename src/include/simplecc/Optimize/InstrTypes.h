#ifndef SIMPLECC_OPTIMIZE_INSTRTYPES_H
#define SIMPLECC_OPTIMIZE_INSTRTYPES_H
#include <simplecc/Parse/AST.h>
#include "simplecc/Optimize/Instruction.h"

namespace simplecc {
class UnaryInstruction : public Instruction {
protected:
  UnaryInstruction(Type *Ty, unsigned iType, Value *V, Instruction *IB = nullptr)
      : Instruction(Ty, iType, 1, IB) {
    setOperand(0, V);
  }
  UnaryInstruction(Type *Ty, unsigned iType, Value *V, BasicBlock *IAE)
      : Instruction(Ty, iType, 1, IAE) {
    setOperand(0, V);
  }

public:
  static bool InstanceCheck(const Instruction *I) {
    return I->getOpcode() == Instruction::Alloca ||
        I->getOpcode() == Instruction::Load;
  }
  static bool InstanceCheck(const Value *V) {
    return IsInstance<Instruction>(V) && InstanceCheck(static_cast<const Instruction *>(V));
  }
};

class BinaryOperator : public Instruction {
protected:
  BinaryOperator(unsigned iType, Value *S1, Value *S2, Type *Ty,
                 std::string Name, Instruction *InsertBefore);
  BinaryOperator(unsigned iType, Value *S1, Value *S2, Type *Ty,
                 std::string Name, BasicBlock *InsertAtEnd);
  friend class Instruction;
  BinaryOperator *cloneImpl() const;

public:
  static BinaryOperator *Create(unsigned Op, Value *S1, Value *S2,
                                std::string = "",
                                Instruction *InsertBefore = nullptr);

  static BinaryOperator *Create(unsigned Op, Value *S1, Value *S2,
                                std::string Name, BasicBlock *InsertAtEnd);

  /// TODO: Use Instruction.def and add a bunch of CreateXXX here.

  bool swapOperands();

  static bool InstanceCheck(const Instruction *I) {
    return I->isBinaryOp();
  }
  static bool InstanceCheck(const Value *V) {
    return IsInstance<Instruction>(V) &&
        InstanceCheck(static_cast<const Instruction *>(V));
  }
};

class CmpInst : public Instruction {
public:
  enum Predicate {
    ICMP_EQ = 32,
    ICMP_NE = 33,
    ICMP_SGT = 38,
    ICMP_SGE = 39,
    ICMP_SLT = 40,
    ICMP_SLE = 41,
    FISRT_ICMP_PREDICATE = ICMP_EQ,
    LAST_ICMP_PREDICATE = ICMP_SLE,
    BAD_ICMP_PREDICATE = ICMP_SLE + 1,
  };
protected:
  CmpInst(Type *Ty, unsigned Op, Predicate pred,
          Value *LHS, Value *RHS, std::string Name = "",
          Instruction *InsertBefore = nullptr);

  CmpInst(Type *Ty, unsigned Op, Predicate pred,
          Value *LHS, Value *RHS, std::string Name,
          BasicBlock *InsertAtEnd);

public:
  static CmpInst *Create(unsigned Op,
                         Predicate predicate, Value *S1,
                         Value *S2, std::string Name = "",
                         Instruction *InsertBefore = nullptr);

  static CmpInst *Create(unsigned Op, Predicate predicate, Value *S1,
                         Value *S2, std::string Name, BasicBlock *InsertAtEnd);

  Predicate getPredicate() const {
    return Pred;
  }
  void setPredicate(Predicate P) { Pred = P; }

  static const char *getPredicateName(Predicate P);

  Predicate getInversePredicate() const {
    return getInversePredicate(getPredicate());
  }

  static Predicate getInversePredicate(Predicate pred);

private:
  Predicate Pred;
};
}
#endif //SIMPLECC_OPTIMIZE_INSTRTYPES_H
