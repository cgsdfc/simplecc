#ifndef SIMPLECC_IR_GLOBALVARIABLE_H
#define SIMPLECC_IR_GLOBALVARIABLE_H
#include <simplecc/IR/Value.h>

namespace simplecc {
class Module;

class GlobalVariable : public Value {
public:
  GlobalVariable(unsigned Size, Module *M);
  unsigned getSize() const {
    return Size;
  }
private:
  unsigned Size;
};

}
#endif //SIMPLECC_IR_GLOBALVARIABLE_H
