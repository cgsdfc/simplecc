#ifndef SIMPLECC_IR_IRCONTEXT_H
#define SIMPLECC_IR_IRCONTEXT_H
#include "simplecc/IR/Type.h"
#include "Type.h"

namespace simplecc {
class IRContext {
  friend class Type;
  Type VoidTy;
  Type LabelTy;
  Type IntTy;
  Type PointerType;
public:
  IRContext();
};

}

#endif //SIMPLECC_IR_IRCONTEXT_H
