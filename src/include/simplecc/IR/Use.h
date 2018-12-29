#ifndef SIMPLECC_IR_USE_H
#define SIMPLECC_IR_USE_H

namespace simplecc {
class Value;
class User;

/// This class represents an edge in the use-def chain.
class Use {
public:
  Use(Value *V, User *U) : Val(V), TheUser(U) {}
  Use() : Val(), TheUser() {}
  Use(const Use &U) = default;
  User *getUser() const { return TheUser; }
  unsigned getOperandNo() const;
  operator Value *() const { return Val; }
  Value *get() const { return Val; }
  Value *operator->() { return Val; }
  const Value *operator->() const { return Val; }
  Value *operator=(Value *RHS) {
    set(RHS);
    return *this;
  }
  const Use &operator=(const Use &RHS) {
    set(RHS.Val);
    return *this;
  }

  /// Change the Value to V
  void set(Value *V) ;

private:
  /// The Value being used.
  Value *Val;
  /// The User that use the Value.
  User *TheUser;
};
}

#endif //SIMPLECC_IR_USE_H
