#ifndef SIMPLECC_OPTIMIZE_USER_H
#define SIMPLECC_OPTIMIZE_USER_H
#include "simplecc/Optimize/Value.h"
#include <vector>

namespace simplecc {
class Type;

/// User represents a Value that can use other Value's as operands.
class User : public Value {
public:
  using OperandListType = std::vector<Use>;
  /// Iterator interface of Operands.
  using op_iterator = OperandListType::iterator;
  using const_op_iterator = OperandListType::const_iterator;

  op_iterator op_begin() { return Operands.begin(); }
  op_iterator op_end() { return Operands.end(); }
  const_op_iterator op_begin() const { return Operands.begin(); }
  const_op_iterator op_end() const { return Operands.end(); }

  OperandListType &getOperandList() { return Operands; }
  const OperandListType &getOperandList() const { return Operands; }

  Value *getOperand(unsigned I) const;
  void setOperand(unsigned I, Value *Val);
  unsigned getNumOperands() const { return Operands.size(); }

  const Use &getOperandUse(unsigned I) const;
  Use &getOperandUse(unsigned I);

  User(const User &) = delete;
  User &operator=(const User &) = delete;

protected:
  User(Type *Ty, unsigned ValTy, unsigned NumOps) : Value(Ty, ValTy) {
    Operands.reserve(NumOps);
  }

private:
  OperandListType Operands;

};
}

#endif //SIMPLECC_OPTIMIZE_USER_H
