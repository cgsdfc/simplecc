
#include <simplecc/IR/IRContext.h>

#include "simplecc/IR/IRContext.h"
#include "simplecc/IR/Constant.h"

using namespace simplecc;

StringLiteral *IRContext::getStringLiteral(const std::string &Str) {
  auto Iter = StringPool.find(Str);
  if (Iter != StringPool.end())
    return Iter->second;
  auto Pos = StringPool.emplace(Str, new StringLiteral(Str));
  return Pos.first->second;
}

ConstantInt *IRContext::getConstantInt(int Int) {
  auto Iter = IntPool.find(Int);
  if (Iter != IntPool.end())
    return Iter->second;
  auto Pos = IntPool.emplace(Int, new ConstantInt(Int));
  return Pos.first->second;
}
