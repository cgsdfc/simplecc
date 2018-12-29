#include <simplecc/IR/Use.h>
#include <simplecc/IR/Value.h>
#include <simplecc/IR/User.h>

using namespace simplecc;

/// Change the used Value to V.
void Use::set(Value *V) {
  if (Val)
    Val->removeUse(*this);
  Val = V;
  TheUser->addUse(*this);
}