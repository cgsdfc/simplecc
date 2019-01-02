#ifndef SIMPLECC_IR_CONSTANT_H
#define SIMPLECC_IR_CONSTANT_H
#include "simplecc/IR/Value.h"

namespace simplecc {
class IRContext;
/// This class represents a constant signed integer literal.
/// The underlying integer should fit into an C++ int.
class ConstantInt : public Value {
  friend class Value;
  friend class IRContext;
  explicit ConstantInt(int Val);
  ~ConstantInt() = default;
public:
  static ConstantInt *get(IRContext &Context, int Val);

  bool isTrue() const { return !isFalse(); }
  bool isFalse() const { return IntVal == 0; }
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
  friend class IRContext;
  explicit StringLiteral(const std::string &Str);
  ~StringLiteral() = default;

public:
  static StringLiteral *get(IRContext &Context, const std::string &Str);
  const std::string &getStrVal() const { return StrVal; }
  static bool InstanceCheck(const Value *V) {
    return V->getValueID() == StringLiteralVal;
  }

private:
  std::string StrVal;
};

} // namespace simplecc

#endif // SIMPLECC_IR_CONSTANT_H