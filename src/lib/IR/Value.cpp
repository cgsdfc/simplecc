#include "simplecc/IR/Value.h"
#include "simplecc/IR/Constant.h"
#include "simplecc/IR/Argument.h"
#include "simplecc/IR/BasicBlock.h"
#include "simplecc/IR/Function.h"
#include "simplecc/IR/Instructions.h"
#include "simplecc/IR/GlobalVariable.h"
#include <algorithm>

using namespace simplecc;

void Value::addUse(const Use &U) {
  assert(this == static_cast<const Value *>(U) &&
      "Val of Use must point to this");
  UseList.push_back(U);
}

void Value::removeUse(const Use &U) {
  UseList.erase(std::remove(UseList.begin(), UseList.end(), U), UseList.end());
}

// TODO: this fails to delete all Instruction class.
void Value::deleteValue() {
  switch (getValueID()) {
#define HANDLE_VALUE(CLASS) \
  case CLASS##Val: delete static_cast<CLASS *>(this); break;
#include "simplecc/IR/Value.def"

  default:assert(false && "Unhandled Enum Value");
  }
}

Value::~Value() {}