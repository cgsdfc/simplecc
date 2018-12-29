#ifndef SIMPLECC_IR_CONSTANT_H
#define SIMPLECC_IR_CONSTANT_H
#include "simplecc/IR/Value.h"

namespace simplecc {
class Constant : public Value {

};

class ConstantString : public Constant {

};

class ConstantInt : public Constant {

};
}

#endif //SIMPLECC_IR_CONSTANT_H