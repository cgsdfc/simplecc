#ifndef SIMPLECC_IR_GLOBALVARIABLE_H
#define SIMPLECC_IR_GLOBALVARIABLE_H
#include "simplecc/IR/Value.h"

namespace simplecc {
class Module;

/// GlobalVariable is a list of int. Its type is PointerTy.
class GlobalVariable : public Value {
  GlobalVariable(unsigned Size, Module *M);

public:
  static GlobalVariable *Create(unsigned Size, Module *M = nullptr) {
    return new GlobalVariable(Size, M);
  }
  unsigned getSize() const { return Size; }
  static bool InstanceCheck(const Value *V) {
    return GlobalVariableVal == V->getValueID();
  }

private:
  unsigned Size;
};

} // namespace simplecc
#endif // SIMPLECC_IR_GLOBALVARIABLE_H