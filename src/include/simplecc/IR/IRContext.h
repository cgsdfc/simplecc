#ifndef SIMPLECC_IR_IRCONTEXT_H
#define SIMPLECC_IR_IRCONTEXT_H
#include "simplecc/IR/Type.h"

#include <string>
#include <unordered_map>

namespace simplecc {
class StringLiteral;
class ConstantInt;

class IRContext {
public:
  IRContext() = default;
  StringLiteral *getStringLiteral(const std::string &Str);
  ConstantInt *getConstantInt(int Int);
private:
  std::unordered_map<int, ConstantInt *> IntPool;
  std::unordered_map<std::string, StringLiteral *> StringPool;
};

} // namespace simplecc

#endif // SIMPLECC_IR_IRCONTEXT_H