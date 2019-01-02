#ifndef SIMPLECC_IR_CONSTANT_H
#define SIMPLECC_IR_CONSTANT_H
#include "simplecc/IR/Value.h"

namespace simplecc {
/// This class represents a constant signed integer literal.
/// The underlying integer should fit into an C++ int.
class ConstantInt : public Value {
  friend class Value;
  explicit ConstantInt(int Val);
  ~ConstantInt() = default;

public:
  static ConstantInt *Create(int Val) { return new ConstantInt(Val); }

  bool isOneVal() const { return IntVal == 1; }
  bool isZeroVal() const { return IntVal == 0; }
  int getIntVal() const { return IntVal; }

  static bool InstanceCheck(const Value *V) {
    return V->getValueID() == ConstantIntVal;
  }

private:
  int IntVal;
};

/// This class represents a string literal. The primary usage of
/// a StringLiteral is to be printed. Its type is PointerType.
class StringLiteral : public Value {
  friend class Value;
  explicit StringLiteral(const std::string &Str);
  ~StringLiteral() = default;

public:
  static StringLiteral *Create(const std::string &Str) {
    return new StringLiteral(Str);
  }

  const std::string &getStrVal() const { return StrVal; }
  static bool InstanceCheck(const Value *V) {
    return V->getValueID() == StringLiteralVal;
  }

private:
  std::string StrVal;
};

} // namespace simplecc

#endif // SIMPLECC_IR_CONSTANT_H