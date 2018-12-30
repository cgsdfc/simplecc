#ifndef SIMPLECC_IR_TYPE_H
#define SIMPLECC_IR_TYPE_H

namespace simplecc {
class IRContext;

/// The Type class is a simplified representation.
/// There is no derived types or composite types. All types are primitive
/// and are represented by this class through its tag member. The reasons why
/// Type is still implemented as a class (not directly a tag in the Value) are:
/// 1. Type has it own methods.
/// 2. make interfaces of both Type and Value cleaner.
/// 3. stand more aligned with llvm.
/// 4. future extension makes easier.
class Type final {
public:
  /// The tagger enum.
  enum TypeKind : unsigned {
#define HANDLE_TYPE(Class, Str) Class##Kind,
#include "simplecc/IR/Type.def"
  };

  /// Accessors to Type instances.
  /// For example, use Type::getVoidType() to get the void type.
#define HANDLE_TYPE(Class, Str) static Type *get##Class();
#include "simplecc/IR/Type.def"

  /// Type identification.
  /// For example, use Ty->isVoidType() to tell if it is the void type.
#define HANDLE_TYPE(Class, Str)                                                \
  bool is##Class() const { return getTypeID() == Class##Kind; }
#include "simplecc/IR/Type.def"

  TypeKind getTypeID() const { return TyKind; }

  /// The name of this Type.
  const char *getName() const;

private:
  TypeKind TyKind;
  Type(TypeKind Kind) : TyKind(Kind) {}
};
} // namespace simplecc

#endif // SIMPLECC_IR_TYPE_H