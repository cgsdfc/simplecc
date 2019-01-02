#ifndef SIMPLECC_IR_USER_H
#define SIMPLECC_IR_USER_H
#include "simplecc/IR/Value.h"
#include <cassert>
#include <vector>

namespace simplecc {
/// User represents a Value that can use other Values as operands.
class User : public Value {
public:
  using OperandListType = std::vector<Use>;
  using op_iterator = OperandListType::iterator;
  using const_op_iterator = OperandListType::const_iterator;

  op_iterator op_begin() { return Operands.begin(); }
  op_iterator op_end() { return Operands.end(); }
  const_op_iterator op_begin() const { return Operands.begin(); }
  const_op_iterator op_end() const { return Operands.end(); }

  unsigned getNumOperands() const { return Operands.size(); }

  iterator_range<op_iterator> operands() {
    return make_range(op_begin(), op_end());
  }

  iterator_range<const_op_iterator> operands() const {
    return make_range(op_begin(), op_end());
  }
  OperandListType &getOperandList() { return Operands; }

  const OperandListType &getOperandList() const { return Operands; }

  Value *getOperand(unsigned I) const { return getOperandUse(I); }

  void setOperand(unsigned I, Value *Val) {
    assert(I < getNumOperands() && "# operand out of range");
    // this uses Val. old Use of this
    Operands[I] = Val;
  }

  const Use &getOperandUse(unsigned I) const {
    assert(I < getNumOperands() && "# operand out of range");
    return Operands[I];
  }

  Use &getOperandUse(unsigned I) {
    return const_cast<Use &>(const_cast<const User *>(this)->getOperandUse(I));
  }

  User(const User &) = delete;
  User &operator=(const User &) = delete;

protected:
  User(Type *Ty, unsigned ValTy, unsigned NumOps) : Value(Ty, ValTy) {
    Operands.resize(NumOps);
  }
  ~User() = default;

private:
  OperandListType Operands;
};
} // namespace simplecc

#endif // SIMPLECC_IR_USER_H
