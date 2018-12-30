#include "simplecc/IR/Constant.h"

using namespace simplecc;

ConstantInt::ConstantInt(int Val)
    : Value(Type::getIntType(), ConstantIntVal), IntVal(Val) {}

StringLiteral::StringLiteral(const std::string &Str)
    : Value(Type::getPointerType(), StringLiteralVal), StrVal(Str) {}