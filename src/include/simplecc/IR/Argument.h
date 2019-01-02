#ifndef SIMPLECC_IR_ARGUMENT_H
#define SIMPLECC_IR_ARGUMENT_H
#include "simplecc/IR/Value.h"
#include <cassert>

namespace simplecc {
class Function;

/// This class represents a formal argument in a Function.
/// Argument Type is always int.
class Argument : public Value {
  Argument(Function *F, unsigned ArgNo);

public:
  static Argument *Create(Function &F, unsigned ArgNo) {
    return new Argument(&F, ArgNo);
  }

  const Function *getParent() const { return Parent; }
  Function *getParent() { return Parent; }

  unsigned getArgNo() const {
    assert(Parent && "can't get number of arg without parent");
    return ArgNo;
  }
  static bool InstanceCheck(const Value *V) {
    return V->getValueID() == ArgumentVal;
  }

private:
  Function *Parent;
  unsigned ArgNo;
};
} // namespace simplecc

#endif // SIMPLECC_IR_ARGUMENT_H
