#ifndef SIMPLECC_IR_GLOBALVARIABLE_H
#define SIMPLECC_IR_GLOBALVARIABLE_H
#include <simplecc/IR/Value.h>

namespace simplecc {
class Module;

/// GlobalVariable is a list of int. Its type is PointerTy.
class GlobalVariable : public Value {
  GlobalVariable(unsigned Size, Module *M);
public:
  GlobalVariable *Create(unsigned Size, Module *M = nullptr);
  unsigned getSize() const {
    return Size;
  }
private:
  unsigned Size;
};

}
#endif //SIMPLECC_IR_GLOBALVARIABLE_H
