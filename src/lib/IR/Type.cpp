#include <simplecc/IR/Type.h>
#include <cassert>

using namespace simplecc;

/// Implement the instance getters
#define HANDLE_TYPE(Class, Str)                                                \
  Type *Type::get##Class() {                                                   \
    static Type The##Class(Class##Kind);                                       \
    return &The##Class;                                                        \
  }
#include "simplecc/IR/Type.def"

const char *Type::getName() const {
  switch (getTypeID()) {
  default:assert(false && "Invalid Enum Value");
#define HANDLE_TYPE(Class, Str)                                                \
  case Class##Kind:                                                            \
    return Str;
#include "simplecc/IR/Type.def"
  }
}