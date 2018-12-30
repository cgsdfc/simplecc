#include <algorithm>
#include <cassert>
#include <simplecc/IR/Value.h>

#include "simplecc/IR/Value.h"

using namespace simplecc;

void Value::addUse(const Use &U) {
  assert(this == static_cast<const Value *>(U) && "Val of Use must point to this");
  UseList.push_back(U);
}

void Value::removeUse(const Use &U) {
  UseList.erase(std::remove(UseList.begin(), UseList.end(), U), UseList.end());
}

void Value::deleteValue() {

}

Value::~Value() {

}