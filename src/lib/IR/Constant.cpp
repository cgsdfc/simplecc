#include "simplecc/IR/Constant.h"
#include "simplecc/IR/IRContext.h"

using namespace simplecc;

ConstantInt::ConstantInt(int Val)
    : Value(Type::getIntType(), ConstantIntVal), IntVal(Val) {}

StringLiteral::StringLiteral(const std::string &Str)
    : Value(Type::getPointerType(), StringLiteralVal), StrVal(Str) {}

StringLiteral *StringLiteral::get(IRContext &Context, const std::string &Str) {
  return Context.getStringLiteral(Str);
}

ConstantInt *ConstantInt::get(IRContext &Context, int Val) {
  return Context.getConstantInt(Val);
}