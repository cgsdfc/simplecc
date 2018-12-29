#include <simplecc/IR/Type.h>
#include "simplecc/IR/IRContext.h"

using namespace simplecc;

Type *Type::getVoidTy(IRContext &Context) {
  return &Context.VoidTy;
}

Type *Type::getLabelTy(IRContext &Context) {
  return &Context.LabelTy;
}

Type *Type::getIntTy(IRContext &Context) {
  return &Context.IntTy;
}

Type *Type::getPointerTy(IRContext &Context) {
  return &Context.PointerType;
}


