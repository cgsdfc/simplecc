#include "simplecc/IR/Module.h"
#include <simplecc/IR/GlobalVariable.h>

using namespace simplecc;

GlobalVariable::GlobalVariable(unsigned Size, Module *M)
    : Value(Type::getPointerType(), GlobalVariableVal), Size(Size) {
  if (M) {
    M->getGlobalList().push_back(this);
  }
}