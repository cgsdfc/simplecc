#ifndef SIMPLECC_IR_ARGUMENT_H
#define SIMPLECC_IR_ARGUMENT_H
#include "simplecc/IR/Value.h"
#include <cassert>

namespace simplecc {
class Function;

class Argument : public Value {
  Function *Parent;
  unsigned ArgNo;
  friend class Function;
public:
  explicit Argument(Type *Ty, Function *F = nullptr, unsigned ArgNo = 0);
  const Function *getParent() const { return Parent; }
  Function *getParent() { return Parent; }
  void setParent(Function *F);
  unsigned getArgNo() const {
    assert(Parent && "can't get number of arg without parent");
    return ArgNo;
  }
  static bool InstanceCheck(const Value *V) {
    return V->getValueID() == ArgumentVal;
  }
};
}

#endif //SIMPLECC_IR_ARGUMENT_H
