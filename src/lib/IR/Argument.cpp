#include "simplecc/IR/Argument.h"

using namespace simplecc;

Argument::Argument(Function *F, unsigned ArgNo)
    : Value(Type::getIntType(), ArgumentVal), Parent(F), ArgNo(ArgNo) {
  assert(F && "Parent cannot be null");
}
